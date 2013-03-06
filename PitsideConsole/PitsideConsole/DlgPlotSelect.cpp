    #include "Stdafx.h"
    #include "DlgPlotSelect.h"
    #include "resource.h"
    #include "PitsideConsole.h"
    #include "LapReceiver.h"
    #include "ArtSQL/ArtSQLite.h"
    #include "LapData.h" // for GetDataChannelName
	#include "DlgRaceSelect.h"

    PlotPrefs m_PlotPrefs[];
    int iPlotView[50];
    float fMinValue[50];
    float fMaxValue[50];
    bool fCancelled = false;
    TCHAR szTemp[512];
	map<int,CExtendedLap*> m_mapLaps; // maps from iLapId to a lap object
    CExtendedLap* m_pReferenceLap;
    ArtListBox m_sfLapList;
    vector<CExtendedLap*> GetAllLaps();


void LoadLaps(ILapReceiver* pReceiver, int m_iRaceId)
  {
    vector<const ILap*> laps = pReceiver->GetLaps(m_iRaceId);
    m_pReferenceLap = NULL;
    for(int x = 0;x < laps.size(); x++)
    {
		const ILap* pLap = laps[x];
		// we don't have this lap yet, so let's put it in
		CExtendedLap* pNewLap = new CExtendedLap(pLap, m_pReferenceLap, pReceiver, true);
		m_mapLaps[pLap->GetLapId()] = pNewLap;
    }
  }

vector<CExtendedLap*> GetAllLaps()
  {
    set<LPARAM> setSelectedLaps = m_sfLapList.GetSelectedItemsData();
    vector<CExtendedLap*> lstLaps;
    for(map<int,CExtendedLap*>::const_iterator i = m_mapLaps.begin(); i != m_mapLaps.end(); i++)
    {
      CExtendedLap* pLap = i->second;
      lstLaps.push_back(pLap);
    }

    return lstLaps;
  }

    int CPlotSelectDlg::InitPlotPrefs(HWND hWnd, LPARAM lParam)
		{
		//  We need to get all of the Y-axis data channels being displayed
		// First load all laps and get their channels
		vector<RACEDATA> lstRaces = m_pLapDB->GetRaces();
		//	Pull the data channels for each lap from this race instance m_iRaceId
		m_pPlotResults->iPlotId = m_iRaceId;	//	Get the Selected Race ID.
		m_sfLapList.Clear();	//  Clear list of laps in memory and reload them.
		if (m_pLapDB->GetLapCount(m_pPlotResults->iPlotId) > 1)	//	If race has more than 1 lap, let's get the laps / channels
		{
			LoadLaps(m_pLapDB, m_pPlotResults->iPlotId);	//	Load all of the laps for this Race ID
			GetAllLaps();	//	Load up lstLaps with all available laps
		}
		//	Now that we have the lap list from the DB, let's get the data channels in them

		m_sfYAxis.Clear();
		set<LPARAM> setYSelected;
		vector<const ILap*> lstLaps = m_pLapDB->GetLaps(m_pPlotResults->iPlotId);
		int TotalYChannels = 1;
		for(int x = 0; x < lstLaps.size(); x++)
		{
			// do like I did above to build up the data channel set
			TCHAR szDataChannelName[MAX_PATH];
			set<DATA_CHANNEL> channels = m_pLapDB->GetAvailableChannels(lstLaps[x]->GetLapId()); // get all the channels that this lap has
			for(set<DATA_CHANNEL>::const_iterator i = channels.begin(); i != channels.end(); i++) // loop through them, insert them into our "all data channels" set
			{
				setYSelected.insert(*i);
				GetDataChannelName(*i, szDataChannelName, NUMCHARS(szDataChannelName));
				m_sfYAxis.AddString(szDataChannelName,*i);
				wcscpy(m_PlotPrefs[x].m_ChannelName, (LPCWSTR)szDataChannelName);
			}
		}
		TotalYChannels = setYSelected.size();
//		for (int z=0; z=TotalYChannels; z++)
//		{
//			const IDataChannel* pChannel = m_pLapDB->GetDataChannel(lstLaps[z]->GetLapId(), setYSelected);
//		}
	for (int i=TotalYChannels + 1; i <= 50; i++)
      {
        LPCWSTR i_ChannelName = L"Channel not used";
        wcscpy(m_PlotPrefs[i].m_ChannelName, (LPCWSTR)i_ChannelName);
      }
      for (int i=1; i <= 50; i++)
      {
        m_PlotPrefs[i].iPlotView = true;  //  Default to display as a graph
        m_PlotPrefs[i].fMinValue = -1.0;    //  Set all lower limits to -1.0
        m_PlotPrefs[i].fMaxValue = 1000000.0;  //  Set all upper limits to 1000000.0
      }
    //  Display all of the data channels, along with the Alarm limits.
      HWND p_hWnd;
      TCHAR szTemp[512];
      szTemp[0] = *m_PlotPrefs[1].m_ChannelName;
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL1);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)szTemp);
      *szTemp = *m_PlotPrefs[2].m_ChannelName;
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL2);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[3].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL3);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[4].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL4);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[5].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL5);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[6].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL6);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[7].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL7);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[8].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL8);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[9].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL9);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[10].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL10);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[11].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL11);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[12].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL12);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[13].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL13);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[14].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL14);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[15].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL15);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[16].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL16);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[17].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL17);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[18].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL18);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[19].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL19);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      *szTemp = m_PlotPrefs[20].m_ChannelName[0];
      p_hWnd = GetDlgItem(hWnd, IDC_PLOTTYPE_CHANNEL20);
      SendMessage(p_hWnd, WM_SETTEXT, 0, (LPARAM)*szTemp);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH1, IDC_PLOTTYPE_VALUE1, IDC_PLOTTYPE_GRAPH1);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH2, IDC_PLOTTYPE_VALUE2, IDC_PLOTTYPE_GRAPH2);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH3, IDC_PLOTTYPE_VALUE3, IDC_PLOTTYPE_GRAPH3);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH4, IDC_PLOTTYPE_VALUE4, IDC_PLOTTYPE_GRAPH4);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH5, IDC_PLOTTYPE_VALUE5, IDC_PLOTTYPE_GRAPH5);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH6, IDC_PLOTTYPE_VALUE6, IDC_PLOTTYPE_GRAPH6);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH7, IDC_PLOTTYPE_VALUE7, IDC_PLOTTYPE_GRAPH7);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH8, IDC_PLOTTYPE_VALUE8, IDC_PLOTTYPE_GRAPH8);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH9, IDC_PLOTTYPE_VALUE9, IDC_PLOTTYPE_GRAPH9);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH10, IDC_PLOTTYPE_VALUE10, IDC_PLOTTYPE_GRAPH10);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH11, IDC_PLOTTYPE_VALUE11, IDC_PLOTTYPE_GRAPH11);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH12, IDC_PLOTTYPE_VALUE12, IDC_PLOTTYPE_GRAPH12);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH13, IDC_PLOTTYPE_VALUE13, IDC_PLOTTYPE_GRAPH13);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH14, IDC_PLOTTYPE_VALUE14, IDC_PLOTTYPE_GRAPH14);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH15, IDC_PLOTTYPE_VALUE15, IDC_PLOTTYPE_GRAPH15);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH16, IDC_PLOTTYPE_VALUE16, IDC_PLOTTYPE_GRAPH16);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH17, IDC_PLOTTYPE_VALUE17, IDC_PLOTTYPE_GRAPH17);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH18, IDC_PLOTTYPE_VALUE18, IDC_PLOTTYPE_GRAPH18);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH19, IDC_PLOTTYPE_VALUE19, IDC_PLOTTYPE_GRAPH19);
      CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH20, IDC_PLOTTYPE_VALUE20, IDC_PLOTTYPE_GRAPH20);

      TCHAR szText[MAX_PATH];
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[1].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT1, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[1].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT1, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[2].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT2, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[2].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT2, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[3].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT3, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[3].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT3, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[4].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT4, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[4].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT4, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[5].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT5, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[5].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT5, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[6].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT6, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[6].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT6, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[7].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT7, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[7].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT7, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[8].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT8, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[8].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT8, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[9].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT9, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[9].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT9, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[10].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT10, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[10].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT10, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[11].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT11, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[11].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT11, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[12].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT12, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[12].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT12, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[13].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT13, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[13].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT13, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[14].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT14, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[14].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT14, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[15].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT15, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[15].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT15, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[16].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT16, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[16].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT16, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[17].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT17, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[17].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT17, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[18].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT18, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[18].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT18, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[19].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT19, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[19].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT19, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[20].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT20, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[20].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT20, LPCWSTR(&szText));

      return 0;
    }

      int SetPlotPrefs(HWND hWnd, set<DATA_CHANNEL> setAvailable)
      {
       //  Display all of the limits for all data channels
      TCHAR szText[MAX_PATH];
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[1].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT1, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[1].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT1, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[2].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT2, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[2].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT2, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[3].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT3, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[3].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT3, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[4].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT4, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[4].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT4, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[5].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT5, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[5].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT5, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[6].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT6, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[6].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT6, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[7].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT7, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[7].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT7, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[8].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT8, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[8].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT8, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[9].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT9, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[9].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT9, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[10].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT10, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[10].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT10, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[11].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT11, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[11].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT11, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[12].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT12, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[12].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT12, LPCWSTR(&szText));       

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[13].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT13, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[13].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT13, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[14].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT14, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[14].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT14, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[15].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT15, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[15].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT15, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[16].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT16, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[16].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT16, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[17].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT17, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[17].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT17, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[18].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT18, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[18].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT18, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[19].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT19, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[19].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT19, LPCWSTR(&szText));

      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[20].fMinValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT20, LPCWSTR(&szText));       
      swprintf (szText, NUMCHARS(szText), L"%9.4f%", m_PlotPrefs[20].fMaxValue);
      SetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT20, LPCWSTR(&szText));

    //  Display all of the data channel names.

      *szTemp = m_PlotPrefs[1].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL1, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[2].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL2, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[3].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL3, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[4].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL4, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[5].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL5, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[6].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL6, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[7].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL7, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[8].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL8, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[9].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL9, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[10].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL10, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[11].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL11, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[12].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL12, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[13].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL13, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[14].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL14, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[15].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL15, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[16].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL16, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[17].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL17, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[18].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL18, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[19].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL19, (LPCWSTR)*szTemp);
      *szTemp = m_PlotPrefs[20].m_ChannelName[0];
      SetDlgItemText(hWnd, IDC_PLOTTYPE_CHANNEL20, (LPCWSTR)*szTemp);
      if (m_PlotPrefs[1].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH1, IDC_PLOTTYPE_VALUE1, IDC_PLOTTYPE_GRAPH1);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH1, IDC_PLOTTYPE_VALUE1, IDC_PLOTTYPE_VALUE1);
      if (m_PlotPrefs[2].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH2, IDC_PLOTTYPE_VALUE2, IDC_PLOTTYPE_GRAPH2);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH2, IDC_PLOTTYPE_VALUE2, IDC_PLOTTYPE_VALUE2);
      if (m_PlotPrefs[3].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH3, IDC_PLOTTYPE_VALUE3, IDC_PLOTTYPE_GRAPH3);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH3, IDC_PLOTTYPE_VALUE3, IDC_PLOTTYPE_VALUE3);
      if (m_PlotPrefs[4].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH4, IDC_PLOTTYPE_VALUE4, IDC_PLOTTYPE_GRAPH4);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH4, IDC_PLOTTYPE_VALUE4, IDC_PLOTTYPE_VALUE4);
      if (m_PlotPrefs[5].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH5, IDC_PLOTTYPE_VALUE5, IDC_PLOTTYPE_GRAPH5);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH5, IDC_PLOTTYPE_VALUE5, IDC_PLOTTYPE_VALUE5);
      if (m_PlotPrefs[6].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH6, IDC_PLOTTYPE_VALUE6, IDC_PLOTTYPE_GRAPH6);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH6, IDC_PLOTTYPE_VALUE6, IDC_PLOTTYPE_VALUE6);
      if (m_PlotPrefs[7].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH7, IDC_PLOTTYPE_VALUE7, IDC_PLOTTYPE_GRAPH7);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH7, IDC_PLOTTYPE_VALUE7, IDC_PLOTTYPE_VALUE7);
      if (m_PlotPrefs[8].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH8, IDC_PLOTTYPE_VALUE8, IDC_PLOTTYPE_GRAPH8);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH8, IDC_PLOTTYPE_VALUE8, IDC_PLOTTYPE_VALUE8);
      if (m_PlotPrefs[9].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH9, IDC_PLOTTYPE_VALUE9, IDC_PLOTTYPE_GRAPH9);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH9, IDC_PLOTTYPE_VALUE9, IDC_PLOTTYPE_VALUE9);
      if (m_PlotPrefs[10].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH10, IDC_PLOTTYPE_VALUE10, IDC_PLOTTYPE_GRAPH10);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH10, IDC_PLOTTYPE_VALUE10, IDC_PLOTTYPE_VALUE10);
      if (m_PlotPrefs[11].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH11, IDC_PLOTTYPE_VALUE11, IDC_PLOTTYPE_GRAPH11);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH11, IDC_PLOTTYPE_VALUE11, IDC_PLOTTYPE_VALUE11);
      if (m_PlotPrefs[12].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH12, IDC_PLOTTYPE_VALUE12, IDC_PLOTTYPE_GRAPH12);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH12, IDC_PLOTTYPE_VALUE12, IDC_PLOTTYPE_VALUE12);
      if (m_PlotPrefs[13].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH13, IDC_PLOTTYPE_VALUE13, IDC_PLOTTYPE_GRAPH13);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH13, IDC_PLOTTYPE_VALUE13, IDC_PLOTTYPE_VALUE13);
      if (m_PlotPrefs[14].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH14, IDC_PLOTTYPE_VALUE14, IDC_PLOTTYPE_GRAPH14);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH14, IDC_PLOTTYPE_VALUE14, IDC_PLOTTYPE_VALUE14);
      if (m_PlotPrefs[15].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH15, IDC_PLOTTYPE_VALUE15, IDC_PLOTTYPE_GRAPH15);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH15, IDC_PLOTTYPE_VALUE15, IDC_PLOTTYPE_VALUE15);
      if (m_PlotPrefs[16].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH16, IDC_PLOTTYPE_VALUE16, IDC_PLOTTYPE_GRAPH16);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH16, IDC_PLOTTYPE_VALUE16, IDC_PLOTTYPE_VALUE16);
      if (m_PlotPrefs[17].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH17, IDC_PLOTTYPE_VALUE17, IDC_PLOTTYPE_GRAPH17);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH17, IDC_PLOTTYPE_VALUE17, IDC_PLOTTYPE_VALUE17);
      if (m_PlotPrefs[18].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH18, IDC_PLOTTYPE_VALUE18, IDC_PLOTTYPE_GRAPH18);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH18, IDC_PLOTTYPE_VALUE18, IDC_PLOTTYPE_VALUE18);
      if (m_PlotPrefs[19].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH19, IDC_PLOTTYPE_VALUE19, IDC_PLOTTYPE_GRAPH19);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH19, IDC_PLOTTYPE_VALUE19, IDC_PLOTTYPE_VALUE19);
      if (m_PlotPrefs[20].iPlotView)
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH20, IDC_PLOTTYPE_VALUE20, IDC_PLOTTYPE_GRAPH20);
      else
        CheckRadioButton(hWnd, IDC_PLOTTYPE_GRAPH20, IDC_PLOTTYPE_VALUE20, IDC_PLOTTYPE_VALUE20);

      return 0;
    }

    LRESULT CPlotSelectDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      switch(uMsg)
      {
        case WM_INITDIALOG:
        {
          //  Initialize all data channels, if not already set by user.
          //  Assumes first data channel (LONG X) will always be a graph
          if (m_PlotPrefs[1].iPlotView == false)
          {
            InitPlotPrefs(hWnd, lParam);
          }
          else
          {
            set<DATA_CHANNEL> setAvailable;
            SetPlotPrefs(hWnd, setAvailable);
          }
            return TRUE;
          break;
        }

        case WM_COMMAND:
        {
          switch(LOWORD(wParam))
          {
          case IDC_PLOTTYPE_GRAPH1:
          {
            m_PlotPrefs[1].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE1:
          {
            m_PlotPrefs[1].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH2:
          {
            m_PlotPrefs[2].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE2:
          {
            m_PlotPrefs[2].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH3:
          {
            m_PlotPrefs[3].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE3:
          {
            m_PlotPrefs[3].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH4:
          {
            m_PlotPrefs[4].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE4:
          {
            m_PlotPrefs[4].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH5:
          {
            m_PlotPrefs[5].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE5:
          {
            m_PlotPrefs[5].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH6:
          {
            m_PlotPrefs[6].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE6:
          {
            m_PlotPrefs[6].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH7:
          {
            m_PlotPrefs[7].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE7:
          {
            m_PlotPrefs[7].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH8:
          {
            m_PlotPrefs[8].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE8:
          {
            m_PlotPrefs[8].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH9:
          {
            m_PlotPrefs[9].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE9:
          {
            m_PlotPrefs[9].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH10:
          {
            m_PlotPrefs[10].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE10:
          {
            m_PlotPrefs[10].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH11:
          {
            m_PlotPrefs[11].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE11:
          {
            m_PlotPrefs[11].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH12:
          {
            m_PlotPrefs[12].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE12:
          {
            m_PlotPrefs[12].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH13:
          {
            m_PlotPrefs[13].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE13:
          {
            m_PlotPrefs[13].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH14:
          {
            m_PlotPrefs[14].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE14:
          {
            m_PlotPrefs[14].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH15:
          {
            m_PlotPrefs[15].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE15:
          {
            m_PlotPrefs[15].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH16:
          {
            m_PlotPrefs[16].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE16:
          {
            m_PlotPrefs[16].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH17:
          {
            m_PlotPrefs[17].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE17:
          {
            m_PlotPrefs[17].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH18:
          {
            m_PlotPrefs[18].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE18:
          {
            m_PlotPrefs[18].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH19:
          {
            m_PlotPrefs[19].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE19:
          {
            m_PlotPrefs[19].iPlotView = false;
            break;
          }
          case IDC_PLOTTYPE_GRAPH20:
          {
            m_PlotPrefs[20].iPlotView = true;
            break;
          }
          case IDC_PLOTTYPE_VALUE20:
          {
            m_PlotPrefs[20].iPlotView = false;
            break;
          }
          case IDOK:
          {
            //  Let's get the values for each channel and store it for program execution
            TCHAR szText[MAX_PATH];
            int len;
            float flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT1));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT1, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[1].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT1));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT1, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[1].fMaxValue = flValue;
           
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT2));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT2, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[2].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT2));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT2, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[2].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT3));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT3, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[3].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT3));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT3, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[3].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT4));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT4, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[4].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT4));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT4, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[4].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT5));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT5, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[5].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT5));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT5, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[5].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT6));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT6, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[6].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT6));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT6, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[6].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT7));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT7, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[7].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT7));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT7, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[7].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT8));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT8, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[8].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT8));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT8, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[8].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT9));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT9, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[9].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT9));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT9, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[9].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT10));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT10, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[10].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT10));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT10, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[10].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT11));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT11, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[11].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT11));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT11, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[11].fMaxValue = flValue;
           
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT12));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT12, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[12].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT12));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT12, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[12].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT13));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT13, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[13].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT13));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT13, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[13].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT14));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT14, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[14].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT14));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT14, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[14].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT15));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT15, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[15].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT15));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT15, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[15].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT16));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT16, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[16].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT16));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT16, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[16].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT17));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT17, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[17].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT17));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT17, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[17].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT18));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT18, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[18].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT18));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT18, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[18].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT19));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT19, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[19].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT19));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT19, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[19].fMaxValue = flValue;

            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_LOWLIMIT20));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_LOWLIMIT20, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[20].fMinValue = flValue;
            len = GetWindowTextLength(GetDlgItem(hWnd, IDC_PLOTTYPE_HIGHLIMIT20));
            GetDlgItemText(hWnd, IDC_PLOTTYPE_HIGHLIMIT20, szText, len+1);
            flValue = _wtof(szText);
            m_PlotPrefs[20].fMaxValue = flValue;
           
            m_pPlotResults->fCancelled = false;
            EndDialog(hWnd,0);
            return TRUE;
          }
          case IDCANCEL:
                m_pPlotResults->fCancelled = true;
            EndDialog(hWnd,0);
            return TRUE;
          }
          break;
        } // end WM_COMMAND
        case WM_CLOSE:
        {
            m_pPlotResults->fCancelled = true;
          EndDialog(hWnd,0);
          break;
        }
      return FALSE;
      }
    }
