#include "stdafx.h"
#include "DashWare.h"
#include <fstream>
#include "arttools.h"
#include <algorithm>

extern ILapReceiver* g_pLapDB;

namespace DashWare
{
  bool SortLapsByTime(const ILap* lap1, const ILap* lap2)
  {
    return lap1->GetStartTime() < lap2->GetStartTime();
  }
  void WriteChannelHeaders(wofstream& out, map<DATA_CHANNEL, const IDataChannel*>& mapData)
  {
    // if you update this function, update the dashware.xml file too!
    out << L"Lap,Time"/*,x,y"*/;

    // column headers
    for(map<DATA_CHANNEL, const IDataChannel*>::iterator i = begin(mapData); i != end(mapData); i++)
    {
      TCHAR szDataChannelName[MAX_PATH];
      const DATA_CHANNEL eChannel = i->first;
      GetDataChannelName(eChannel, szDataChannelName, NUMCHARS(szDataChannelName));
      out << "," << szDataChannelName;
    }
    out << "," << endl;

  }

  //	Subprogram saves the chosen lap's data out to a CSV file for import into rendering programs like Dashware and RaceRender
  HRESULT SaveToDashware(LPCTSTR lpszFilename, /*const vector<const ILap*>& lstLaps,*/ vector<CExtendedLap*> lstLaps1)
  {
    //	lstLaps: Vector of chosen laps to output, lpszFileaname: Filename to output CSV file to
//	if(lstLaps.size() <= 0) return E_FAIL;
	if(lstLaps1.size() <= 0) return E_FAIL;
/*
    vector<const ILap*> lstSortedLaps = lstLaps;	//	Get all of the chosen laps
    sort(begin(lstSortedLaps),end(lstSortedLaps),SortLapsByTime);	//	Sort all of the chosen laps by lap time
*/
    wofstream out;
    out.open(lpszFilename);	//	Open up the CSV file for input

    map<DATA_CHANNEL,const IDataChannel*> dwMapChannels;	//	dwMapChannels is the list of Data Channels for all laps

//    for(int ixLap = 0;ixLap < lstLaps.size(); ixLap++)
    for(int ixLap = 0;ixLap < lstLaps1.size(); ixLap++)
    {
	  //	Find all of the populated data channels for this lap
	 set<DATA_CHANNEL> setLapData = lstLaps1[ixLap]->GetAvailableChannels();	//	Load the CExtendedLap data for the lap list
//	  for(int y = 0; y < DATA_CHANNEL_COUNT; y++)
//      {
      for(set<DATA_CHANNEL>::iterator i = setLapData.begin(); i != setLapData.end(); i++)
      {
//		const IDataChannel* pChannel = g_pLapDB->GetDataChannel(lstLaps[ixLap]->GetLapId(),(DATA_CHANNEL)y);		
		const IDataChannel* pChannel = lstLaps1[ixLap]->GetChannel(*i);
        DASSERT(pChannel->IsValid());	//	Ensure that channel doesn't cause execution issues
//        DASSERT(pChannel->IsLocked() && pChannel->IsValid());
//        if(pChannel && pChannel->IsLocked() && pChannel->IsValid())
        if(pChannel && pChannel->IsValid())
        {
//          dwMapChannels[(DATA_CHANNEL)y] = pChannel;	//	Add Data Channel to the list
			dwMapChannels[*i] = pChannel;	//	Add Data Channel to the list
        }
        else
        {
          DASSERT(FALSE);
        }
      }
    }
	WriteChannelHeaders(out, dwMapChannels);	//	Put all Data Channel names into the header for the CSV file
    
	int msLastLine = 0;	//	Indexer for pulling correct timestamp data for each data channel
    float flStartTime = 0; // start time in seconds;
//    for (int ixLap = 0; ixLap < lstSortedLaps.size(); ixLap++)
    for (int ixLap = 0; ixLap < lstLaps1.size(); ixLap++)
    {
//      const ILap* pLap = lstSortedLaps[ixLap];	//	Get the chosen lap
      const ILap* pLap = lstLaps1[ixLap]->GetLap();	//	Get the chosen lap
      int msStartTime = INT_MAX; // start time and end time for this lap (gotten by looking at start and end time for data channels)
      int msEndTime = -INT_MAX;
/*
					//	Go through all of the mapped data channels to determine the msStart and msEnd times for this lap, based upon all data available
					  for(int y = 0; y < DATA_CHANNEL_COUNT; y++)
					  {
			//			const IDataChannel* pChannel = lstLaps1[ixLap]->GetChannel((DATA_CHANNEL)y);
						  const IDataChannel* pChannel = g_pLapDB->GetDataChannel(pLap->GetLapId(),(DATA_CHANNEL)y);
						DASSERT(pChannel->IsValid());	//	Ensure that channel doesn't cause execution issues
			//			  DASSERT(pChannel->IsLocked() && pChannel->IsValid());
			//			  if(pChannel && pChannel->IsLocked() && pChannel->IsValid())
						if(pChannel && pChannel->IsValid())
						{
						  dwMapChannels[(DATA_CHANNEL)y] = pChannel;
						}
						else
						{
						  DASSERT(FALSE);
						}
					 }
			//	  WriteChannelHeaders(out, dwMapChannels);	//	Put all Data Channel names into the header for the CSV file
*/
//      const IDataChannel* pDC = dwMapChannels[DATA_CHANNEL_VELOCITY];	//	Use the Velocity data channel to set time boundary conditions for this lap
	  const IDataChannel* pDC = lstLaps1[ixLap]->GetChannel(DATA_CHANNEL_VELOCITY);
	  //	Go through all of the mapped data channels to determine the msStart and msEnd times for this lap, based upon all data available
/*	  for(map<DATA_CHANNEL, const IDataChannel*>::iterator i = begin(dwMapChannels); i != end(dwMapChannels); i++)
      {
//		const IDataChannel* pDC = g_pLapDB->GetDataChannel(pLap->GetLapId(),(DATA_CHANNEL)i->first);	//	This is a huge memory hog
        const IDataChannel* pDC = dwMapChannels[i->first];	//	Not sure if this gives the correct lap times
        if(pDC)
        {
          msStartTime = min(pDC->GetStartTimeMs(),msStartTime);
//          msEndTime = max(pDC->GetEndTimeMs(),msEndTime);
        }
      }
*/
      msStartTime = pDC->GetStartTimeMs();
	  msEndTime = max(msEndTime, msStartTime + pLap->GetTime()*1000);

	  const vector<TimePoint2D>& lstPoints = pLap->GetPoints();	  //	List of points for this lap
	  float flRunningAverage[DATA_CHANNEL_COUNT] = {0.0f};
      bool fUseRunningAverage[DATA_CHANNEL_COUNT] = {0};
      fUseRunningAverage[DATA_CHANNEL_X_ACCEL] = true;
      fUseRunningAverage[DATA_CHANNEL_Y_ACCEL] = true;
      fUseRunningAverage[DATA_CHANNEL_Z_ACCEL] = true;

//      for(int msQuery = msStartTime; msQuery < msEndTime; msQuery += 2000)	//	Increased time movement for debug purposes
      for(int msQuery = msStartTime; msQuery < msEndTime; msQuery += 100)
      {
        if(msQuery > msLastLine)
        {
		  //	Lap counter
		  out << ixLap << ",";
		  //	Time
          TCHAR szTemp[100] = L"0";
          _snwprintf(szTemp,NUMCHARS(szTemp),L"%6.3f",((float)msQuery/1000.0f)+flStartTime);
          out << szTemp;
/*		  //	Position X
		  _snwprintf(szTemp,NUMCHARS(szTemp),L"%0.1f",0.0f);
          TimePoint2D pt = ::GetPointAtTime(lstPoints,msQuery);
          _snwprintf(szTemp,NUMCHARS(szTemp),L"%4.6f",pt.flX);
          out << "," << szTemp;
		  //	Position Y
          _snwprintf(szTemp,NUMCHARS(szTemp),L"%1.1f",0.0f);
		  _snwprintf(szTemp,NUMCHARS(szTemp),L"%4.6f",pt.flY);
          out << "," << szTemp;
*/
		  //	For all of the hard coded channels, loop through them to get the data channel value at this point in time
//		  const ILap* pLap_temp = g_pLapDB->GetLap(pLap->GetLapId());	//	Get the data for this lap/channel
//		  for(map<DATA_CHANNEL,const IDataChannel*>::iterator i = begin(dwMapChannels); i != end(dwMapChannels); i++)
		  for(map<DATA_CHANNEL,const IDataChannel*>::iterator i = begin(dwMapChannels); i != end(dwMapChannels); i++)
          {
//			const IDataChannel* pDC_temp = g_pLapDB->GetDataChannel(pLap->GetLapId(),(DATA_CHANNEL)i->first);	//	Get the data for this lap/channel
//            const IDataChannel* pDC = i->second;	//	Not sure if this gives the correct lap data
            const IDataChannel* pDC = lstLaps1[ixLap]->GetChannel(i->first);;	//	Not sure if this gives the correct lap data
            if(pDC)
            {
              float flValue = pDC->GetValue(msQuery);
              if(fUseRunningAverage[i->first])
              {
                flRunningAverage[i->first] = 0.7*flValue + 0.3*flRunningAverage[i->first];
              }
              _snwprintf(szTemp,NUMCHARS(szTemp),L"%5.6f",fUseRunningAverage[i->first] ? flRunningAverage[i->first] : flValue);
//              _snwprintf(szTemp, NUMCHARS(szTemp), L"%5.6f", flValue);	//	Used for debug purposes
              out << "," << szTemp;	//	Output the value to the CSV file
            }
            else
            {
              out << ","; // if this lap didn't include the data channel, skip it
            }
          }
          out << "," << endl << flush;
          msLastLine = msQuery;	//	Index the time in preparation for the next lap
        }
//		msLastLine = 0;		//	Reset the last line query, but this would allow out of time order laps
      }
    }

    out.close();

    return S_OK;
  }
}