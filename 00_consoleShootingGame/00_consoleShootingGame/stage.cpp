#include <iostream>

#include "stage.h"
#include "enemy.h"
#include "player.h"

// ��ü �������� ����
int maxStageNum = 0;

//////////////////////////////////////////////////////////////
// �������� ���ϸ��� ��� stageInfo.txt�� �о���� �Լ�
// createEnemy(fileName) ȣ��
// createPlayer ȣ��
// 
//////////////////////////////////////////////////////////////
void loadStageInfo(const int stageNum)
{
	// ���� �б�
	FILE* stageInfoFile;
	errno_t err;
	// �������� ���� ����
	err = fopen_s(&stageInfoFile, "stageInfo.txt", "r");
	// ���� �ε� ����
	if (err != 0)
	{
		return;
	}
	// �������� ���� ����
	int fileCnt = 0;
	// �������� ��ü ���� �б�
	fscanf_s(stageInfoFile, "%d ", &fileCnt);
	// ��ü �������� �� ������Ʈ
	maxStageNum = fileCnt;
	// ���� �������� ��ȣ�� �´� ���ϸ��� �о�´�.
	int iCnt;
	char fileName[15];
	for (iCnt = 1; iCnt <= fileCnt; ++iCnt)
	{
		fscanf_s(stageInfoFile, "%s", fileName, sizeof(fileName));
		if (iCnt == stageNum)
		{
			break;
		}
	}
	// �� ����
	createEnemy(fileName);
	// �÷��̾� ����
	createPlayer();
	// ���� �ݱ�
	fclose(stageInfoFile);
}
