#pragma once
//#include <cv.h>
//#include <highgui.h>
#include <opencv2/opencv.hpp>
typedef struct 
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;
} Visited;

class CBlobLabeling
{
public:
	CBlobLabeling(void);
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;				// ���̺��� ���� �̹���
	int			m_nThreshold;			// ���̺� ������Ȧ��(�Ӱ谪)
	Visited*	m_vPoint;				// ���̺��� �湮����
	int			m_nBlobs;				// ���̺��� ����


	CvRect*		m_recBlobs;				// �� ���̺� ����


	int* areaSize;



public:
	// ���̺� �̹��� ����(�̹���, �Ӱ谪)
	void		SetParam(IplImage* image, int nThreshold);

	// ���̺�(����)
	void		DoLabeling();

private:
	// ���̺�(�̹���, �Ӱ谪)
	int		 Labeling(IplImage* image, int nThreshold);

	// ����Ʈ �ʱ�ȭ(�ʺ�, ����)
	void	 InitvPoint(int nWidth, int nHeight);
	void	 DeletevPoint();

	// ���̺� ��� ���(���̺� ��ȣ, �ӽù���, �ʺ�, ����)
	void	 DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// ���̺�(���� �˰���) (����, �ʺ�, ����, �Ӱ谪)
	int		_Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold);
	
	// _Labling ���� ��� �Լ� 
	// ���� ���� ����(����, �ʺ�, ����, X,Y,����X��, ���� Y��, ��X��, �� Y��)
	int		__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	// ���� ���� (����, ����X��, ���� Y��, ��X��, ��Y��, �ʺ�, ����)
	int		__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);

	// ����, ���� ũ�Ⱑ �񱳰� ���� ���� ���̺� ����
	// ������ ������ ���� ���̺� ����
public:
	void	BlobSmallSizeConstraint(int nWidth, int nHeight);
private:
	void	_BlobSmallSizeConstraint(int nWidth, int nHeight, CvRect* rect, int* nRecNumber);

	// ����, ���� ũ�Ⱑ �񱳰� ���� ū ���̺� ����
	// ������ ������ ū ���̺� ����
public:
	void	BlobBigSizeConstraint(int nWidth, int nHeight);
private:
	void	_BlobBigSizeConstraint(int nWidth, int nHeight, CvRect* rect, int* nRecNumber);


};
