/*           < 비트맵 파일 구조>
			14바이트- 비트맵 파일 헤더
			40바이트-비트맵 정보 헤더	
			가변-색상 테이블
			가변-픽셀 데이터
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <io.h>
#pragma pack(push, 1)
#define PIXEL_SIZE 3		  // 픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN 4		  // 픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨;
typedef struct bitmapheader
{
	unsigned short bttype;	  //BMP 파일 매직 넘버. 비트맵 파일이 맞는지 확인하는데 사용하며 ASCII 코드로 0x42(B), 0x4D(M)가 저장됩니다.
	unsigned int btsize;	  //파일 크기(바이트)
	unsigned short reserved1; /*현재는 사용하지 않으며 미래를 위해 예약된 공간   */
	unsigned short reserved2; /*  현재는 사용하지 않으며 미래를 위해 예약된 공간 */
	unsigned btoffBits;		  //비트맵 데이터의 시작 위치
} BITMAPHEADER;
struct bitmapinfoheader
{
	unsigned int biSize;		 //현재 비트맵 정보 헤더(BITMAPINFOHEADER)의 크기
	int biWidth;				 //비트맵 이미지의 가로 크기(픽셀)
	int biHeight;				 //비트맵 이미지의 세로 크기(픽셀).
	unsigned short biPlanes;	 //사용하는 색상판의 수. 항상 1입니다.
	unsigned short biBitCount;	 //픽셀 하나를 표현하는 비트 수
	unsigned int biCompression;	 //압축 방식. 보통 비트맵은 압축을 하지 않으므로 0입니다.
	unsigned int biSizeImage;	 //비트맵 이미지의 픽셀 데이터 크기
	int biXPelsPerMeter;		 //그림의 가로 해상도(미터당 픽셀)
	int biYPelsPerMeter;		 //그림의 세로 해상도(미터당 픽셀)
	unsigned int biClrUsed;		 //색상 테이블에서 실제 사용되는 색상 수
	unsigned int biClrImportant; //비트맵을 표현하기 위해 필요한 색상 인덱스 수
};
typedef struct bitmaprgb
{
	unsigned char rgbtBlue;		 //bule
	unsigned char rgbtGreen;	 //green
	unsigned char rgbtRed;		 //red
} RGB;
#pragma pack(pop)
int main()
{
	FILE *fpBmp;															//비트맵 파일 포인터
	FILE *fpTxt;															// 텍스트 파일 포인터
	FILE *test;																//테스트 파일 포인터
	BITMAPHEADER fileHeader;												// 비트맵 파일 헤더 구조체 변수
	bitmapinfoheader infoHeader;											// 비트맵 정보 헤더 구조체 변수
	char *image;															//이미지 포인터 선언
	int size;																// 픽셀의 크기
	int width, height;														// 전체 픽셀의 가로,세로
	int padding;															// 빈공간
	char ascii[] = {'#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' '}; //명안차이로 출력
	char c = 0;																//이미지를 출력하기 위한 변수

	//비트맵 파일 열기
	fpBmp = fopen("sample50x50.bmp", "rb");									//비트맵 파일을 바이너리 모드로 열기 ,--> //sample, sample50x50, Peppers80x80.bmp
	if (fpBmp == NULL)													    //파일을 열지 못 할 경우
	{
		return 0;
	}
	if (fread(&fileHeader, sizeof(BITMAPHEADER), 1, fpBmp) < 1)				// 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료 , fread는 잘 읽으면 1을 반환
	{
		fclose(fpBmp);
		return 0;
	}
	if (fileHeader.bttype != 'MB')										    //매직 넘버를 확인
	{
		fclose(fpBmp);
		return 0;
	}
	if (fread(&infoHeader, sizeof(bitmapinfoheader), 1, fpBmp) < 1)			 // 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료 , fread는 잘 읽으면 1을 반환
	{
		fclose(fpBmp);
		return 0;
	}
	if (infoHeader.biBitCount != 24)										//24비트맵이 아닐 경우
	{
		fclose(fpBmp);
		return 0;
	}
	size = infoHeader.biSizeImage;
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	
	if (fopen("asci.txt", "r") != NULL )									//이미 파일이 있을 경우
	{
		remove("asci.txt");
	}
	//나머지공간 계산
	/*(27 * PIXEL_SIZE) % PIXEL_ALIGN
	(27*3)%4  
	(81)%4
	������ = 1
	*/

	padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;
	if (size == 0)																	//픽셀의 크기가 0일 경우  크기를 다시 계산
	{
		size = (width * PIXEL_SIZE + padding) * height;
	}
	image = (char *)malloc(size);													 //이미지를 동적할당

	fseek(fpBmp, fileHeader.btoffBits, SEEK_SET);
	if (fread(image, size, 1, fpBmp) < 1)
	{
		fclose(fpBmp);
		return 0;
	}
	fclose(fpBmp);
	fpTxt = fopen("asci.txt", "w");													//텍스트 파일을 열기(만들고)
	if (fpTxt == NULL)
	{
		free(image);
		return 0;
	}
	//이미지를 ascii 텍스트화
	for (int y = height - 1; y >= 0; y--)											//C는 0부터 시작이기 때문에 1980*720 은 0~1979 ,세로
	{
		for (int x = 0; x < width; x++) //가로
		{
			int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);
			RGB *pixel = (RGB *)&image[index];
			unsigned char blue = pixel->rgbtBlue;
			unsigned char green = pixel->rgbtGreen;
			unsigned char red = pixel->rgbtRed;
			unsigned char gray = (red + green + blue) / PIXEL_SIZE;					//RGB평균값을 구하면 흑백값을 구할 수 있다.
			char c = ascii[gray * sizeof(ascii) / 256];
			fprintf(fpTxt, "%c%c", c, c);
		}
		fprintf(fpTxt, "\n");
	}
	
	while (c  != EOF)
	{
		c = fgetc(fpTxt);
		putchar(c);
	}
	fclose(fpTxt);
	free(image);
}
