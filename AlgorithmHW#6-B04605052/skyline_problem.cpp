// Algorithms HW#6 B04605052 資管四 施星宇
#include <iostream>
#include <string> // 需要些許的字串處理
#include <fstream> // 要開啟檔案用
#include <algorithm> // 其中的max()會用到
using namespace  std;

struct strip // struct一個type:strip，儲存兩個整數
{ 
    int left; // 整數left用來儲存skyline中高度變化的位置
    int ht;   // 整數ht用來儲存skyline中高度變化的高度
}; 

const int DATA_SIZE = 2;
// 用變數DATA_SIZE儲存初始的檔案格式，因為一個building的資訊可以用兩個strip儲存，因此DATA_SIZE設為2
// ex:一個為2 6 7的building可以轉換為{2,6}、{7,0}這兩個strip來儲存
void dataOutput(strip mergeResult[], int resultNum); // 依據strip陣列，輸出skyline的資料
int mergeSkyLine(strip* skyLineList[], int skyLineListNum[], strip mergeResult[], int n, int partNum); 
// 先將輸入的building分成兩部分，分別再做mergeSkyLine()，分到最小單位後再執行merge()將兩串skyLine merge成一串
int merge(strip PartOne[], strip PartTwo[], strip mergeResult[], int partOneNum, int partTwoNum); // 用來將兩串skyLine merge成一串

// 演算法說明：本演算法採用Divide and Conquer的概念解決Skyline Problem，在演算法開始前先將輸入的資料
// 轉換為strip的格式，每兩個strip構成一個由一個building形成的skyline，並儲存於skyLineList陣列中，接
// 著透過mergeSkyLine()將skyLineList中的資料分成兩半，分成兩半後再次呼叫mergeSkyLine()，再將資料再
// 分成兩半，直到只剩一組資料，此時每一筆資料都是僅由一個building構成的skyline，可視為base case，接著
// 呼叫merge()將skyline兩兩互相合併，此時可視為induction step，直到所有的skyline都合併為一skyline
// 則演算法結束，並輸出結果。

int main (int argc, char const *argv[]) // 執行main()調用演算法，其中argv[1]儲存了要開啟的資料的檔名
{
	// char str[255]; // 先產生一字串str，用來儲存將輸入的檔名與".txt"合併之結果
	// char b[5] = ".txt"; // 產生另一字串b，用來儲存".txt"字串
 //    strcat(str,argv[1]); // 將輸入的檔名放入字串str中
 //    strcat(str,b); // 再將字串b，也就是".txt"接在其後面，形成完整的檔名
	freopen(argv[1], "rt", stdin); // 最後調用freopen()開啟相對應檔名的資料
	int n = 0; // 產生一變數n用來儲存輸入的building的數量
	cin >> n;  // 輸入此n值
	int** buildingRange = new int* [n]; // buildingRange指標陣列用來儲存輸入的building所在的位置範圍
	int* buildingHeight = new int [n];  // buildingHeight陣列用來儲存輸入的building其高度
	for (int i = 0; i < n; i++) // 輸入n筆building資料
	{ 
		buildingRange[i] = new int[DATA_SIZE]; // 在buildingRange指標下建立一為DATA_SIZE長度的陣列，儲存building的range
		cin >> buildingRange[i][0] >> buildingHeight[i] >> buildingRange[i][1]; // 輸入building數值
	}
	strip** skyLineList = new strip* [n]; // 建立一skyLineList指標陣列，共有n列，以strip來儲存不同building形成的skyline
	int* skyLineListNum = new int [n];    // 一維skyLineListNum陣列用來儲存skyLineList中相對應的skyline其中所含的strip數
	for (int i = 0; i < n; i++) // 將building的資料轉為strip，儲存於skyLineList中
	{
		skyLineList[i] = new strip[2]; 
		skyLineList[i][0].left = buildingRange[i][0];
		skyLineList[i][0].ht = buildingHeight[i];
		skyLineList[i][1].left = buildingRange[i][1];
		skyLineList[i][1].ht = 0;
		skyLineListNum[i] = 2; // 一個building由兩個strip所構成，因此設陣列長度為2
	}
	strip* mergeResult = new strip [2 * n]; // 建立一長度為2 * n之strip陣列，用來儲存skyline的結果
	int resultNum = mergeSkyLine(skyLineList, skyLineListNum, mergeResult, n, n);
// ，這裡正式執行演算法，用mergeSkyLine()將輸入的資料skyLineList陣列分為兩個部分，而定一變數resultNum儲存merge後的skyline的長度
	dataOutput(mergeResult, resultNum); // 將結果輸出
	return 0;
}

int mergeSkyLine(strip* skyLineList[], int skyLineListNum[], strip mergeResult[], int n, int partNum)
//  mergeSkyLine()用來處理Divide and conquer 中的Divide的部分，將資料分成兩份之後，對每份資料再次呼叫mergeSkyLine()，將資料再細
//  分成兩份，直到不可分割後，將回傳的strip陣列呼叫merge()將其合併，最後則會得到結果
{	
	int resultNum = 0;  //  用來儲存回傳的結果的長度
	if (partNum != 1)   //  用partNum變數來儲存切分後的資料數，若partNum != 1則代表還沒分割到不可分割
	{
		if (partNum % 2 == 0)  //  若分割後的資料數是偶數
		{
			partNum = partNum / 2;  //  則將partNum令為其原來值得1/2
			strip** skyLineListOne = new strip* [partNum];  //  新建一個二維動態strip指標陣列用來儲存分割後的資料
			strip** skyLineListTwo = new strip* [partNum];
			int* skyLineListNumOne = new int[partNum]; //  新建一個一位整數陣列用來儲存分割後資料中，每一份資料中skyline的strip數量
			int* skyLineListNumTwo = new int[partNum];
			for (int i = 0; i <= (n / 2) - 1; i++)  //  將其中1/2的資料複製到skyLineListOne陣列中
			{
				skyLineListOne[i] = new strip[skyLineListNum[i]]; //  將指標陣列指向一維陣列
				for (int j = 0; j < skyLineListNum[i]; j++)  //  複製資料
				{
					skyLineListOne[i][j] = skyLineList[i][j];
				}
				skyLineListNumOne[i] = skyLineListNum[i];  //  儲存該資料的strip數量
			}
			for (int i = n / 2; i <= n - 1; i++)  //  將其中1/2的資料複製到skyLineListTwo陣列中
			{
				skyLineListTwo[i - n / 2] = new strip[skyLineListNum[i]];  //  將指標陣列指向一維陣列
				for (int j = 0; j < skyLineListNum[i]; j++)  //  複製資料
				{
					skyLineListTwo[i - n / 2][j] = skyLineList[i][j];
				}
				skyLineListNumTwo[i - n / 2] = skyLineListNum[i];  //  儲存該資料的strip數量
			}
			strip* mergePartOne = new strip[2 * n];  //  創建一維Strip陣列用來儲存其中一部分的skyline結果
			strip* mergePartTwo = new strip[2 * n];  //  創建一維Strip陣列用來儲存令一部分的skyline結果
			int resultNumOne = mergeSkyLine(skyLineListOne, skyLineListNumOne, mergePartOne, partNum, partNum); 
			//  而用整數resultNumOne儲存回傳的strip陣列的長度
			int resultNumTwo = mergeSkyLine(skyLineListTwo, skyLineListNumTwo, mergePartTwo, partNum, partNum);
			resultNum = merge(mergePartOne, mergePartTwo, mergeResult, resultNumOne, resultNumTwo);  
			//  呼叫merge()函數進行結果的合併
		}
		else  //  如果分割後的資料數是奇數
		{
			partNum = partNum / 2;  //  一樣將partNum令為其原來值得1/2
			n = n - 1;  //  並將原資料的總數減一，因此n會變回偶數 
			strip** skyLineListOne = new strip* [partNum];  //  新建一個二維動態strip指標陣列用來儲存分割後的資料
			strip** skyLineListTwo = new strip* [partNum];
			int* skyLineListNumOne = new int[partNum];  //  新建一個一位整數陣列用來儲存分割後資料中，每一份資料中skyline的strip數量
			int* skyLineListNumTwo = new int[partNum];
			for (int i = 0; i <= (n / 2) - 1; i++)  //  將其中1/2的資料複製到skyLineListOne陣列中
			{
				skyLineListOne[i] = new strip[skyLineListNum[i]];  //  將指標陣列指向一維陣列
				for (int j = 0; j < skyLineListNum[i]; j++)  //  複製資料
				{
					skyLineListOne[i][j] = skyLineList[i][j];
				}
				skyLineListNumOne[i] = skyLineListNum[i];  //  儲存該資料的strip數量
			}
			for (int i = n / 2; i <= n - 2; i++)  //  將其中1/2的資料複製到skyLineListTwo陣列中
			{
				skyLineListTwo[i - n / 2] = new strip[skyLineListNum[i]];  //  將指標陣列指向一維陣列
				for (int j = 0; j < skyLineListNum[i]; j++)  //  複製資料
				{
					skyLineListTwo[i - n / 2][j] = skyLineList[i][j];
				}
				skyLineListNumTwo[i - n / 2] = skyLineListNum[i];  //  儲存該資料的strip數量
			}
			strip* temp = new strip[2 * n];  
			//  建立一個strip陣列叫做temp，因為接下來要預先把其中兩組資料merge，資料數才會變回偶數，結果用temp陣列儲存
			int tempNum = merge(skyLineList[n - 1], skyLineList[n], temp, skyLineListNum[n - 1], skyLineListNum[n]);
			//  tempNum用來儲存merge後的strip數量
			skyLineListTwo[n - 1 - n / 2] = new strip[tempNum]; //  skyLineListTwo指向一個新的陣列，用來儲存新merge的strip

			for (int i = 0; i < tempNum; i++) //  複製資料
			{
				skyLineListTwo[n - 1 - n / 2][i] = temp[i];
			}
			skyLineListNumTwo[n - 1 - n / 2] = tempNum; //  儲存該資料的strip數量
			delete [] temp;
			strip* mergePartOne = new strip[2 * n];  //  創建一維Strip陣列用來儲存其中一部分的skyline結果
			strip* mergePartTwo = new strip[2 * n];  //  創建一維Strip陣列用來儲存令一部分的skyline結果
			int resultNumOne = mergeSkyLine(skyLineListOne, skyLineListNumOne, mergePartOne, partNum, partNum);
			//  而用整數resultNumOne儲存回傳的strip陣列的長度
			int resultNumTwo = mergeSkyLine(skyLineListTwo, skyLineListNumTwo, mergePartTwo, partNum, partNum);
			resultNum = merge(mergePartOne, mergePartTwo, mergeResult, resultNumOne, resultNumTwo);
			//  呼叫merge()函數進行結果的合併
		}
	}
	else  //  如果已經分割到不可分割
	{
		for (int i = 0; i < skyLineListNum[0]; i++)
		{
			mergeResult[i] = skyLineList[0][i];  //  將結果直接回傳輸入的資料
		}
		resultNum = skyLineListNum[0];  //  並將resultNum設為輸入資料的strip數
	}
	return resultNum;  //  回傳strip長度值
}

int merge(strip PartOne[], strip PartTwo[], strip mergeResult[], int partOneNum, int partTwoNum)
//  用來merge兩個輸入的strip陣列
{
	bool flag = 1;  //  令一個flag，用來表示是否所有輸入的strip陣列都merge完成
	int pointer1 = 0, pointer2 = 0, h1 = 0, h2 = 0, resultpointer = 0;
	//  pointer1表示陣列中目前merge到哪一組strip，pointer2表示陣列中目前merge到哪一組strip，h1表示目前1號strip陣列的高度值，h2
	//  表示目前2號strip陣列的高度值，resultpointer表示輸出結果陣列的目前的長度
	while (flag)  //  如果陣列沒有merge完成，則持續執行
	{
		if (pointer1 < partOneNum && pointer2 < partTwoNum)  //  如果兩個merge的陣列都還沒做完
		{
			if (PartOne[pointer1].left < PartTwo[pointer2].left)  //  又其中一號陣列中的strip之left較小
			{
				h1 = PartOne[pointer1].ht;  //  h1令為該strip之ht值
				int maxHeight = max(h1, h2);  //  取h1和h2的最大值
				if (resultpointer != 0)  //  若並不是第一個輸出的strip
				{
					if (mergeResult[resultpointer - 1].ht != maxHeight)  //  又前一個strip的高度值，不等於準備要輸入的strip的高度值
																		 //  若相等則不輸出
					{
						mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
						mergeResult[resultpointer].ht = maxHeight;
						resultpointer++;  //  將輸出strip陣列長度加一
					}
				}
				else  //  若是第一個輸出的strip
				{
					mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
					mergeResult[resultpointer].ht = maxHeight;
					resultpointer++;  //  將輸出strip陣列長度加一
				}
				pointer1++;  //  因為是輸出一號陣列，因此指向的位置向後移
			}
			else if (PartOne[pointer1].left == PartTwo[pointer2].left)  //  若一號陣列和二號陣列的值相等
			{
				if (PartOne[pointer1].ht != PartTwo[pointer2].ht)  //  又若兩者的高度值不相等
				{
					h1 = PartOne[pointer1].ht;  //  h1令為一號陣列strip之ht值
					h2 = PartTwo[pointer2].ht;  //  h1令為二號陣列strip之ht值
					int maxHeight = max(h1, h2);  //  取h1和h2的最大值
					if (resultpointer != 0)  //  若並不是第一個輸出的strip
					{
						if (mergeResult[resultpointer - 1].ht != maxHeight)  //  又前一個strip的高度值，不等於準備要輸入的strip的高度值
																			 //  若相等則不輸出
						{
							mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
							mergeResult[resultpointer].ht = maxHeight;
							resultpointer++;  //  將輸出strip陣列長度加一
						}
					}
					else  //  若是第一個輸出的strip
					{
						mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
						mergeResult[resultpointer].ht = maxHeight;
						resultpointer++;  //  將輸出strip陣列長度加一
					}
					pointer1++;  //  一號陣列指向的位置向後移
					pointer2++;  //  二號陣列指向的位置向後移
				}
				else  //  如果兩者高度值相等
				{
					h1 = PartOne[pointer1].ht;  //  h1令為一號陣列strip之ht值
					h2 = PartTwo[pointer2].ht;  //  h1令為二號陣列strip之ht值
					int maxHeight = h1;  //  最大高度值為其中一個
					if (resultpointer != 0)  //  若並不是第一個輸出的strip
					{
						if (mergeResult[resultpointer - 1].ht != maxHeight)  //  又前一個strip的高度值，不等於準備要輸入的strip的高度值
																			 //  若相等則不輸出
						{
							mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
							mergeResult[resultpointer].ht = maxHeight;
							resultpointer++;  //  將輸出strip陣列長度加一
						}
					}
					else  //  若是第一個輸出的strip
					{
						mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
						mergeResult[resultpointer].ht = maxHeight;
						resultpointer++;  //  將輸出strip陣列長度加一
					}
					pointer1++;  //  一號陣列指向的位置向後移
					pointer2++;  //  二號陣列指向的位置向後移
				}
			}
			else  //  若二號陣列的值比一號陣列大
			{
				h2 = PartTwo[pointer2].ht;  //  h2令為該strip之ht值
				int maxHeight = max(h1, h2);  //  取h1和h2的最大值
				if (resultpointer != 0)  //  若並不是第一個輸出的strip
				{
					if (mergeResult[resultpointer - 1].ht != maxHeight)  //  又前一個strip的高度值，不等於準備要輸入的strip的高度值
																		 //  若相等則不輸出
					{
						mergeResult[resultpointer].left = PartTwo[pointer2].left;  //  則輸出strip值
						mergeResult[resultpointer].ht = maxHeight;
						resultpointer++;  //  則輸出strip值
					}
				}
				else  //  若是第一個輸出的strip
				{
					mergeResult[resultpointer].left = PartTwo[pointer2].left;  //  則輸出strip值
					mergeResult[resultpointer].ht = maxHeight;
					resultpointer++;  //  則輸出strip值
				}
				pointer2++;  //  因為是輸出二號陣列，因此指向的位置向後移
			}
		}
		else if(pointer1 >= partOneNum && pointer2 < partTwoNum)  //  若一號陣列先做完了，則直接輸出二號陣列
		{
			mergeResult[resultpointer].left = PartTwo[pointer2].left;  //  則輸出strip值
			mergeResult[resultpointer].ht = PartTwo[pointer2].ht;
			resultpointer++;  //  則輸出strip值
			pointer2++;  //  因為是輸出二號陣列，因此指向的位置向後移
		}
		else if(pointer2 >= partTwoNum && pointer1 < partOneNum)   //  若二號陣列先做完了，則直接輸出一號陣列
		{
			mergeResult[resultpointer].left = PartOne[pointer1].left;  //  則輸出strip值
			mergeResult[resultpointer].ht = PartOne[pointer1].ht;
			resultpointer++;  //  則輸出strip值
			pointer1++;  //  因為是輸出一號陣列，因此指向的位置向後移
		}
		else  //  都做完了
		{
			flag = 0;  //  令flag = 0以跳出回圈
		}
	}
	return resultpointer;  //  回傳結果strip陣列的長度
}

void dataOutput(strip mergeResult[], int resultNum)  //  用來輸出strip陣列
{
	for (int i = 0; i < resultNum; i++)  //  根據strip陣列的長度，要跑resultNum次
	{
		if (i + 1 != resultNum)  //  若還沒輸出到最後一個strip
		{
			cout << mergeResult[i].left << " " << mergeResult[i].ht << " "; // 輸出strip的兩個值，以及空格
		}
		else  //  若輸出到最後一個strip
		{
			cout << mergeResult[i].left;  //  則只輸出到最後一個strip的left值
		}
	}
}