#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include "c_tqsave.h"

// Store the save file into memory
unsigned char *pBuf;
long pSize;

// Path to the original file
char *g_szFilePath;

// Default player stats for level 1 hero
long long defaultStats[] = { 0x442480000, 0x442480000, 0x442480000, 0x443960000, 0x943960000 };
/*
unsigned char headerFemale[62] = {
	0x0D, 0x00, 0x00, 0x00, 0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x56, 0x65,
	0x72, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x70, 0x6C, 0x61, 0x79, 0x65, 0x72, 0x43, 0x68, 0x61, 0x72, 0x61,
	0x63, 0x74, 0x65, 0x72, 0x43, 0x6C, 0x61, 0x73, 0x73, 0x09, 0x00, 0x00,
	0x00, 0x53, 0x6F, 0x72, 0x63, 0x65, 0x72, 0x65, 0x73, 0x73, 0x08, 0x00,
	0x00, 0x00
};

unsigned char headerMale[60] = {
	0x0D, 0x00, 0x00, 0x00, 0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x56, 0x65,
	0x72, 0x73, 0x69, 0x6F, 0x6E, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x70, 0x6C, 0x61, 0x79, 0x65, 0x72, 0x43, 0x68, 0x61, 0x72, 0x61,
	0x63, 0x74, 0x65, 0x72, 0x43, 0x6C, 0x61, 0x73, 0x73, 0x07, 0x00, 0x00,
	0x00, 0x57, 0x61, 0x72, 0x72, 0x69, 0x6F, 0x72, 0x08, 0x00, 0x00, 0x00
};
*/
void TQParseData(TQSAVE_FLAG eFlag, TQSAVEDATA *tq)
{
	if (!pBuf)
		return;

	// Grab the gender
	int *j = (int*)(pBuf + 45);

	// Read/write the gender
	if (eFlag == TQ_READ_DATA) {
		tq->Gender = (TQ_GENDER)*j;
	} // Writing gender disabled for now 

	long k = 0; int c = 1;

	for (long i = 180; i < pSize; i++) 
	{
		// Locate the offset of the money variable
		if (pBuf[i] == 0x6D && pBuf[i + 1] == 0x6F &&
			pBuf[i + 2] == 0x6E && pBuf[i + 3] == 0x65 && pBuf[i + 4] == 0x79) {
			j = (int*)(pBuf + i + 5);

			if (eFlag == TQ_READ_DATA) {
				tq->nMoney = *j;
			}
			else {
				*j = tq->nMoney;
			}
		}
		// Locate the offset of the difficulty variable
		if (pBuf[i] == 0x65 && pBuf[i + 1] == 0x64 && pBuf[i + 2] == 0x54 &&
			pBuf[i + 3] == 0x6F && pBuf[i + 4] == 0x74 && pBuf[i + 5] == 0x61 &&
			pBuf[i + 6] == 0x6C) {

			// Offset to the start of value
			k = i + 6 + 13;

			j = (int*)(pBuf + k);

			if (eFlag == TQ_READ_DATA) {
				tq->gameMode = (TQ_GAME_MODE)*j;
			}
			else {
				*j = tq->gameMode;
			}
		}

		// Masteries
		if (eFlag == TQ_WRITE_DATA && pBuf[i] == 0x6C && pBuf[i + 1] == 0x6F && pBuf[i + 2] == 0x77 &&
			pBuf[i + 3] == 0x65 && pBuf[i + 4] == 0x64) {

			k = i + 4 + 1;
			j = (int*)(pBuf + k);
			*j = 0x02;
		}

		// Byte string that indicates block start
		if (pBuf[i] == 0xCE && pBuf[i + 1] == 0xFA &&
			pBuf[i + 2] == 0x1D && pBuf[i + 3] == 0xB0)
		{

			switch (pBuf[i + 4])
			{
				case 0x0C: // Player name offset
				{
					// Only process one time, sometimes this byte string is duplicated
					if (tq->playerName[0] != 0x00)
						break;

					// Temp var for writing string
					int s = 0;

					// Adjust offset
					k = i + 24;

					for (; (k - (i + 24)) < 28; k++) {
						// End of char string
						if (pBuf[k] == 0x0D)
							break;
						// Skip null bytes
						if (pBuf[k] != 0x00)
							tq->playerName[s++] = pBuf[k];
					}
					i = k;
				}
				break;

				case 0x16: // Player stats offset
				{
					// Adjust offset
					k = i + 30;
					// Character level variable
					j = (int*)(pBuf + k);
					if (eFlag == TQ_READ_DATA)
						tq->nLevel = *j;
					else
						*j = tq->nLevel;

					// Adjust offset
					k = k + 37;
					// Character experience
					j = (int*)(pBuf + k);
					if (eFlag == TQ_READ_DATA)
						tq->nExp = *j;
					else
						*j = tq->nExp;

					// Adjust offet
					k = k + 22;
					// Character stat points
					j = (int*)(pBuf + k);
					if (eFlag == TQ_READ_DATA)
						tq->nStatPts = *j;
					else
						*j = tq->nStatPts;

					// Adjust offset
					k = k + 19;
					// Character skill points
					j = (int*)(pBuf + k);
					if (eFlag == TQ_READ_DATA)
						tq->nSkillPts = *j;
					else
						*j = tq->nSkillPts;

					i = k; // Jump past this section (avoid an infinte loop)
				}
				break;

				case 0x04: // Player chr stats
				{
					// We only do this if it's a write request
					if (eFlag != TQ_WRITE_DATA)
						break;

					// Adjust offset to the first value
					k = i + 4 + 8;
					long long *x = (long long*)(pBuf + k);

					// Reset to default unmodified value
					*x = defaultStats[0];
				
					// Loop through and update the remainder
					for (int i = 0; i <4; i++) {
						k += 12;
						x = (long long*)(pBuf + k);
						*x = defaultStats[i+1];
					}
					i = k;
				}
				break;

				case 0x09: // We have to process this way because skillName isn't unique within the save file
					// Skip over the first two skillName variables and ignore anything after
					if (c > 2 && c < 5) {
						k = i + 4 + 13;

						j = (int*)(pBuf + k);
						if (eFlag == TQ_WRITE_DATA) {
							*j = MASTERY_DEFAULT;
						}
						else {
							switch (c) {
								case 3:
									tq->nMastery1 = *j;
								break;
								
								case 4:
									tq->nMastery2 = *j;
								break;
							}
						}

						i = k;
					}
					c++;
				break;
			}
		}
	}
}

bool TQLoadFile(const char *szPath, TQSAVEDATA *tq)
{
	int s = lstrlen(szPath) + 1;
	g_szFilePath = (char*)malloc(s);
	strcpy(g_szFilePath, szPath);

	FILE *fp = fopen(g_szFilePath, "rb+");
	if (!fp)
		return FALSE;

	fseek(fp, 0, SEEK_END);
	pSize = ftell(fp);
	if (pSize < 1) {
		fclose(fp);
		return FALSE;
	}

	rewind(fp);

	pBuf = (unsigned char*)malloc(pSize);
	memset(pBuf, 0, pSize);

	fread(pBuf, pSize, 1, fp);
	fflush(fp);
	fclose(fp);

	// Check file header
	char header_ver = *(pBuf + 17);
//	if (header_ver != 0x02) {
//		free(pBuf);
//		return FALSE;
//	}

	TQParseData(TQ_READ_DATA, tq);

	return TRUE;
}

bool TQSaveFile(TQSAVEDATA *tq)
{
	BOOL bReturn = FALSE;

	if (tq->nLevel < 1 || tq->nLevel > 85)
		return FALSE;

	TQParseData(TQ_WRITE_DATA, tq);

	FILE *fp = fopen(g_szFilePath, "wb");
	if (fp) {
		/*
		long offset = 0;
		unsigned char *newPtr = 0;

		if (tq->Gender == GENDER_MALE) {
			offset = 60;
			fwrite(headerMale, sizeof(headerMale), 1, fp);
			newPtr = (pBuf + offset);
		} else {
			offset = 60;
			fwrite(headerFemale, sizeof(headerFemale), 1, fp);
			newPtr = (pBuf + offset);
			//offset = 60;
		}*/
		size_t s = fwrite(pBuf, pSize, 1, fp);
		if (s >0 )
			bReturn = TRUE;
		fflush(fp);
		fclose(fp);
	}
	
	return bReturn;
}

void TQCleanUp()
{
	// Free memory
	free(g_szFilePath);
	free(pBuf);
}
