#include "main.h"
#include "c_tqsave.h"

TQSAVEDATA tq = { 0 };
HICON g_hIcon = NULL;
HINSTANCE g_hInstance = 0;

void SetupControls(HWND hWnd) 
{
	HWND hEdtCharName = GetDlgItem(hWnd, IDC_CHARNAME);

	SendMessage(hEdtCharName, EM_SETLIMITTEXT, (WPARAM)14, (LPARAM)0);
	SendMessage(hEdtCharName, WM_SETTEXT, (WPARAM)0, (LPARAM)tq.playerName);

	HWND hTrackBar = GetDlgItem(hWnd, IDC_SLIDER1);

	SendMessage(hTrackBar, TBM_SETTICFREQ, 10, 0);
	SendMessage(hTrackBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 85));

	SendMessage(hTrackBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)tq.nLevel);

	int newPos = SendMessage(hTrackBar, TBM_GETPOS, 0, 0);
	
	SetDlgItemInt(hWnd, IDC_LEVEL, newPos, TRUE);
	SetDlgItemInt(hWnd, IDC_EXP, tq.nExp, TRUE);
	SetDlgItemInt(hWnd, IDC_MONEY, tq.nMoney, TRUE);
	SetDlgItemInt(hWnd, IDC_CHRPTS, tq.nStatPts, TRUE);
	SetDlgItemInt(hWnd, IDC_SKLPTS, tq.nSkillPts, TRUE);

	// Usable controls
	UINT uControls[] = { IDC_MONEY, IDC_RESPEC, IDC_SLIDER1, IDC_NORMAL, IDC_EPIC, IDC_LEG };
	for (int i = 0; uControls[i]; i++)
		EnableWindow(GetDlgItem(hWnd, uControls[i]), TRUE);
	EnableMenuItem(GetMenu(hWnd), ID_FILE_SAVE, TRUE);
	
	// Male/female buttons
	if (tq.Gender == GENDER_MALE) {
		CheckDlgButton(hWnd, IDC_MALE, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_FEMALE, BST_UNCHECKED);
	}
	else {
		CheckDlgButton(hWnd, IDC_FEMALE, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_MALE, BST_UNCHECKED);
	}

	switch (tq.gameMode)
	{
	case GAME_MODE_NORMAL:
		CheckDlgButton(hWnd, IDC_NORMAL, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_EPIC, BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_LEG, BST_UNCHECKED);
		break;

	case GAME_MODE_EPIC:
		CheckDlgButton(hWnd, IDC_NORMAL, BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_EPIC, BST_CHECKED);
		CheckDlgButton(hWnd, IDC_LEG, BST_UNCHECKED);
		break;

	case GAME_MODE_LEG:
		CheckDlgButton(hWnd, IDC_NORMAL, BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_EPIC, BST_UNCHECKED);
		CheckDlgButton(hWnd, IDC_LEG, BST_CHECKED);
		break;
	}

}

INT_PTR CALLBACK HelpDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) 
	{
		case WM_INITDIALOG:
		{
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)g_hIcon);

			SendMessage(GetDlgItem(hWnd, IDC_LOGO), STM_SETICON, (WPARAM)g_hIcon, 0);

			char szVersion[10] = { 0 };
			LoadString(NULL, IDS_VERSION, szVersion, 10);
			SetDlgItemText(hWnd, IDC_VERSION, szVersion);

			ShowWindow(hWnd, SW_SHOW);
		}
		break;

		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
				case NM_CLICK:          // Fall through to the next case.
				case NM_RETURN:
					ShellExecute(NULL, "open", "https://www.patreon.com/timb3r", NULL, NULL, SW_SHOW);
				break;
			}
		break;

		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_ABOUT_CLOSE)
				DestroyWindow(hWnd);
			break;
	
		case WM_DESTROY:
		case WM_QUIT:
		case WM_CLOSE:
			DestroyWindow(hWnd);
		break;

		default:
		return FALSE;
	}
	return TRUE;
}
INT_PTR CALLBACK DialogWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

		case WM_INITDIALOG:
		{
			// Set Icon
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)g_hIcon);

			HWND hTrackBar = GetDlgItem(hWnd, IDC_SLIDER1);

			SendMessage(hTrackBar, TBM_SETTICFREQ, 10, 0);
			SendMessage(hTrackBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 85));
		}
		break;

		case WM_HSCROLL:
		{
			HWND hTrackbar = GetDlgItem(hWnd, IDC_SLIDER1);

			if (hTrackbar == (HWND)lParam)
			{
				// Update trackbar position

				int newPos = SendMessage(hTrackbar, TBM_GETPOS, 0, 0);
				SetDlgItemInt(hWnd, IDC_LEVEL, newPos, TRUE);
				SetDlgItemInt(hWnd, IDC_EXP, TQExpLevels[newPos-1], TRUE);

				int skillPoints = (newPos * 3) - 3;
				int chrPoints = (newPos * 2) - 2;
				if (newPos < 2)
					skillPoints = chrPoints = 0;

				SetDlgItemInt(hWnd, IDC_SKLPTS, skillPoints, TRUE);
				SetDlgItemInt(hWnd, IDC_CHRPTS, chrPoints, TRUE);

			}
		}
		break;

		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED) {
				WORD uID = LOWORD(wParam);
				switch (uID) {
					case IDC_MALE:
					case IDC_FEMALE:
						break;

					case IDC_NORMAL:
						tq.gameMode = GAME_MODE_NORMAL;
						break;
					case IDC_EPIC:
						tq.gameMode = GAME_MODE_EPIC;
						break;
					case IDC_LEG:
						tq.gameMode = GAME_MODE_LEG;
						break;

					case IDC_RESPEC:
						if (!TQSaveFile(&tq))
							MessageBox(hWnd, "Unable to save file!", "Error!", MB_OK | MB_ICONERROR);
						else
							MessageBox(hWnd, "Character file was successfully updated!", "Success!", MB_OK | MB_ICONINFORMATION);
						break;

					case IDC_CLOSE:
						DestroyWindow(hWnd);
						break;
				}
			}
			if (HIWORD(wParam) == EN_CHANGE)
			{	
				WORD uID = LOWORD(wParam);
				int v = GetDlgItemInt(hWnd, uID, NULL, TRUE);

				switch (uID)
				{
					case IDC_MONEY:
					{	if (v > 0)
							tq.nMoney = v;
						else {
							v = 0;
						}
					}
					break;
					
					case IDC_EXP:
						if (v > 0 && v < TQExpLevels[84])
							tq.nExp = v;
					break;

					case IDC_LEVEL:
						if (v > 1 && v < 86)
							tq.nLevel = v;
					break;

					case IDC_CHRPTS:
						if (v > 0)
							tq.nStatPts = v;
					break;

					case IDC_SKLPTS:
						if (v > 0)
							tq.nSkillPts = v;
					break;
					
				}
			}
			if (HIWORD(wParam) == 0) {
				switch (LOWORD(wParam))
				{
					case ID_FILE_OPEN:
					{
						char *szTemp = (char*)malloc(MAX_PATH);
						memset(szTemp, 0, MAX_PATH);

						OPENFILENAME ofn = { 0 };

						ofn.lStructSize     = sizeof(ofn);
						ofn.hwndOwner       = hWnd;
						ofn.lpstrFile       = szTemp;
						ofn.lpstrFile[0]    = '\0';
						ofn.nMaxFile        = MAX_PATH;
						ofn.lpstrFilter     = "TQA Character File (*.chr)\0*.chr\0";
						ofn.nFilterIndex    = 0;
						ofn.lpstrFileTitle  = NULL;
						ofn.nMaxFileTitle   = 0;
						ofn.lpstrInitialDir = "%userprofile%\\Documents\\My Games\\Titan Quest - Immortal Throne\\SaveData\\Main";
						ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

						if (GetOpenFileName(&ofn)) {
							ZeroMemory(&tq, sizeof(TQSAVEDATA));

							if (!TQLoadFile(szTemp, &tq)) {
								MessageBox(hWnd, "Unable to load character file!", "Error!", MB_OK | MB_ICONERROR);
							}
							else {
								SetupControls(hWnd);
							}
							free(szTemp);
						}
					}
					break;

					case ID_FILE_SAVE:
						if (!TQSaveFile(&tq))
							MessageBox(hWnd, "Unable to save file!", "Error!", MB_OK | MB_ICONERROR);
						else
						{
							//char *szBuf[MAX_PATH] = { 0 };
							//sprintf(szBuf, "%s was patched successfully!");
							MessageBox(hWnd, "Character file was successfully updated!", "Success!", MB_OK | MB_ICONINFORMATION);
						}
					break;

					case ID_FILE_EXIT:
						DestroyWindow(hWnd);
					break;

					case ID_HELP_ABOUT:
						CreateDialog(NULL, MAKEINTRESOURCE(IDD_ABOUT), hWnd, HelpDialogProc);
					break;
				}
			}
		break;

		case WM_DESTROY:
		case WM_QUIT:
			PostQuitMessage(0);
		break;

		case WM_CLOSE:
			DestroyWindow(hWnd);	
		break;

		default:
			return FALSE;
	}
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCommand, int nShow)
{
	g_hInstance = hInstance;

	INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
	InitCommonControlsEx(&icc);

	g_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	
	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, DialogWindowProc);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	TQCleanUp();
	return (int)msg.wParam;
}