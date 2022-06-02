#ifndef MYMATH_INCLUDED
#define MYMATH_INCLUDED

#include <windowsx.h>
#include "mycomplex.h"
#include "resource.h"

// Op Type is +,-,*,=,c,t,i(Conj,Trans,Invers)
 BOOL MatrixOperation(Ccomplex  *Src1,int nRow1,int nCol1,
					Ccomplex  *Src2,int nRow2,int nCol2,
					Ccomplex  *Tag,char cOpType,BOOL bWait=TRUE);
 BOOL MatrixOperation(float  *Src1,int nRow1,int nCol1,
					float  *Src2,int nRow2,int nCol2,
					float  *Tag,char cOpType,BOOL bWait=TRUE);
 int FWT(float *fSrcValue,long nSize,BOOL bWait=TRUE);
 int IFWT(float  *fSrcValue,long lItemSize,BOOL bWait=TRUE);
 int FFT(Ccomplex  *fSrcValue,long nItemSize,BOOL bWait=TRUE);
 int IFFT(Ccomplex  *fSrcValue,long lItemSize,BOOL bWait=TRUE);
 int GetPowerSpectrOfFourier(Ccomplex  *fSrcValue,float *fPowerSpectr,long nItemSize);

 int FFT_2nd_Order(int nIsoLength,BYTE *lpBMP,Ccomplex *pcplxSrcValue,BOOL bWait);
 int GetPowerSpectrOf_2nd_Order_Fourier(Ccomplex  *fSrcValue,float *fPowerSpectr,int nIsoLength);
 int IFFT_2nd_Order(int nIsoLength,Ccomplex *pcplxSrcValue,BOOL bWait);
 int IFWT_2nd_Order(int nIsoLength,BYTE *lpBMP,float *pfSrc,BOOL bWait);
 int FWT_2nd_Order(int nIsoLength,BYTE *lpBMP,float *pfSrc,BOOL bWait);

typedef struct tagMEM_INTERMEDIATSTRUCT{
	float  *lpf_r_mk;
	float  *lpf_b_mi;
	float  *lpf_b_mi_;
	float fPm;
	float fSm;
	double dblFPMm;
}MEM_INTERMEDIATSTRUCT,far *LPMEM_INTERMEDIATSTRUCT;


class MEM_SPECTRCLASS_B{
	protected:
		MEM_INTERMEDIATSTRUCT memInter_1,memInter_2;
		float  *lpfP_f;
		float  *lpf_r_mk_FPMm;
		long lm;
		long lM;
		short nAICMin_m;
		short nAICMax_m;
		long lmForMinFPM;
		double dblminFPM;
		double dblmimPm;
		float fSampleTime;
		short nScale;
	public:
		MEM_SPECTRCLASS_B(long lN,int nMax_Pro,int far *nScale_Pro){
			lM = lN/2;
			nAICMin_m = (int)(2*sqrt((double)lN));
			nAICMax_m = (int)(3*sqrt((double)lN));
			if(nMax_Pro <= 100 && nMax_Pro > 0){
				if( nAICMax_m < lM*nMax_Pro/100)
					nAICMax_m = (int)(lM*nMax_Pro/100);
			}
			if(!(*nScale_Pro))
				*nScale_Pro=1;
			lpf_r_mk_FPMm = (float *)GlobalAllocPtr(GHND,(lM+1)*sizeof(float));
			memInter_1.lpf_r_mk = (float *)GlobalAllocPtr(GHND,(lM+1)*sizeof(float));
			memInter_2.lpf_r_mk = (float *)GlobalAllocPtr(GHND,(lM+1)*sizeof(float));
			memInter_1.lpf_b_mi = (float *)GlobalAllocPtr(GHND,(lN)*sizeof(float));
			memInter_2.lpf_b_mi = (float *)GlobalAllocPtr(GHND,(lN)*sizeof(float));
			memInter_1.lpf_b_mi_ = (float *)GlobalAllocPtr(GHND,(lN)*sizeof(float));
			memInter_2.lpf_b_mi_ = (float *)GlobalAllocPtr(GHND,(lN)*sizeof(float));
			if((*nScale_Pro) == 1 )
				lpfP_f = (float  *)GlobalAllocPtr(GHND,sizeof(float)*lN*(*nScale_Pro));
			while((*nScale_Pro) > 1 ){
				if((lpfP_f = (float  *)GlobalAllocPtr(GHND,sizeof(float)*lN*(*nScale_Pro))))
					break;
				(*nScale_Pro)--;
			}
			if( !lpf_r_mk_FPMm || 
				!memInter_1.lpf_r_mk ||
				!memInter_2.lpf_r_mk ||
				!memInter_1.lpf_b_mi ||
				!memInter_2.lpf_b_mi ||
				!memInter_1.lpf_b_mi_ ||
				!memInter_2.lpf_b_mi_ ||
				!lpfP_f){
				MessageBox(GetFocus(),"Cann't Allocate Memory For MEM Spectr Computing","Allocate Memory",MB_OK);
			}
			nScale = (*nScale_Pro);
			
		}
		~MEM_SPECTRCLASS_B(){
			GlobalFreePtr(memInter_1.lpf_r_mk);
			GlobalFreePtr(memInter_2.lpf_r_mk);
			GlobalFreePtr(memInter_1.lpf_b_mi);
			GlobalFreePtr(memInter_2.lpf_b_mi);
			GlobalFreePtr(memInter_1.lpf_b_mi_);
			GlobalFreePtr(memInter_2.lpf_b_mi_);
			GlobalFreePtr(lpfP_f);
		}
		int InitializeFor_m_Eq_1_B(float  *fSrcValue,long lItemSize);
		 int GetMEM_Spectr_B(float  *fSrcValue,long lItemSize,HWND hDlg);
		 void GetPf(long lN,HWND hDlg);
};

class MEM_SPECTRCLASS_YW : public MEM_SPECTRCLASS_B{
	private:
		float  *lpfC_k;
		double dblDeltm_1;
		float fErrEps;
	public:
		MEM_SPECTRCLASS_YW(long lN,int nMax_Pro,int far* nScale_Pro,float fErr) : MEM_SPECTRCLASS_B(lN,nMax_Pro,(int far*)nScale_Pro){
			fErrEps = fErr;
			lpfC_k = (float *)GlobalAllocPtr(GHND,(lM+1)*sizeof(float));
			if(!lpfC_k){
				MessageBox(GetFocus(),"Cann't Allocate Memory For Y-K MEM Spectr Computing","Allocate Memory",MB_OK);
			}
		}
		~MEM_SPECTRCLASS_YW(){
			GlobalFreePtr(lpfC_k);
		}
		
		int InitializeFor_m_Eq_1_YW(float  *fSrcValue,long lItemSize);
		 int GetMEM_Spectr_YW(float  *fSrcValue,long lItemSize,HWND hDlg);
};

//*****************************************************
//
//			Statistics Computation	Class
//			Mathimatical Expected Value And 
//			Power Of Standard Distribution
//			And Standard Distribution                  
//
//*****************************************************
class STATISTICSCLASS{
	private:          
		float fPowOfStandardDistr;
	public:
		float fExpectedValue;
		float fStandardDistr;
		double dblSum;
		double dblPowSum;
	public:
		 BOOL GetStatistics(BYTE *lpcValue,DWORD dwLength);
		 BOOL GetStatistics(char *lpcValue,DWORD dwLength);
		 BOOL GetStatistics(int *lpnValue,DWORD dwLength);
		 BOOL GetStatistics(float *lpnValue,DWORD dwLength);
		 float GetExpectValue(){ return fExpectedValue;}
		 float GetPowOfStdDistr(){ return fPowOfStandardDistr;}
		 float GetStdDistr(){ return fStandardDistr;}
};


class INTERPOLATECLASS{
	private:
		int nN;
		int nX0,nXn,nStep;
		//***** Lagrangue ******
		float far *lpfDivisor;
		//***** Spline ******
		float far *lpfA,far *lpfX,far *lpfB,far *lpfCi;

		char far *lpcY;
		int far *lpnY,far *lpnX;
		float far *lpfY,far *lpfS;
	public:
		//************** Lagrangue Section ************************
		INTERPOLATECLASS(int nX0_,int nXn_,int nStep_){
			nX0 = nX0_,nXn=nXn_,nStep=nStep_;
			PointerInit();
			nN=(nXn-nX0)/nStep+1; 
			lpfDivisor=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN );
			if( !lpfDivisor){
				MessageBox(GetFocus(),"Cann't Allocate Memory For Interpolation","Get Interpolation",MB_OK);
				PostQuitMessage(-1);
			}
			int i,j;
			float far *lpfDivisorP;
			for(i=nX0,lpfDivisorP=lpfDivisor;i<=nXn;i+=nStep,lpfDivisorP++){
				for(j=nX0,(*lpfDivisorP)=(float)1;j<=nXn;j+=nStep){
					if(i==j)	continue;
					(*lpfDivisorP) *= (i-j);
				}
			}
		}
		//************** Spline Section,Equal, char type ************************
		INTERPOLATECLASS(int nX0_,int nXn_,int nStep_,char far *lpcYi){
			lpcY=lpcYi;
			nX0 = nX0_,nXn=nXn_,nStep=nStep_;

			PointerInit();
			nN=(nXn-nX0)/nStep+1; 
			lpfA=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN*nN );
			lpfX=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN );
			lpfB=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN );
			if( !lpfA || !lpfX || !lpfB ){
				MessageBox(GetFocus(),"Cann't Allocate Memory For Interpolation","Get Interpolation",MB_OK);
				PostQuitMessage(-1);
			}
			//****** Make Matrix A and B ************
			int i;
			float far *lpfABak;
				// *** Boundary Condition ******
			*lpfA = (float)2; lpfA[1]=(float).5; 
			*lpfB = (float)(6./(2*nStep)*( (float)(lpcYi[1]-(*lpcYi))/ nStep ));
				// *** Normal Condition ******
			for(i=1,lpfABak=lpfA+nN;i<nN-1;i++,lpfABak+=nN+1){
				*lpfABak = (float).5;//*** Mu,Equal Step...***
				lpfABak[1] = (float)2;//*** Constant ...***
				lpfABak[2] = (float).5;//*** Ramda,Equal Step...***
				lpfB[i] = (float)(6./(2*nStep)*( (float)(lpcYi[i+1]-lpcYi[i])/ nStep 
										-(float)(lpcYi[i]-lpcYi[i-1])/ nStep));
			}
				 // *** Boundary Condition ******
			*lpfABak = (float).5;//*** Mu,Equal Step...***
			lpfABak[1] = (float)2;//*** Constant ...***
			lpfB[i] = (float)(6./(2*nStep)*( -(float)(lpcYi[i]-lpcYi[i-1])/ nStep ));
			//******* Comput Matrix X ************
			if(!MatrixOperation((float *)lpfA,nN,nN,(float *)lpfX,1,nN,(float *)lpfB,'1',FALSE)){
				MessageBox(GetFocus(),"Interpolation Error! Result Is Unreliable!","Spline Interpolation",MB_OK);
			}
			//******* Comput Efficient Values Into Matrix A ****
			for(i=0,lpfCi=lpfA; i<nN-1; i++,lpfCi += 3){
				lpfCi[1] = lpfX[i]/2;
				lpfCi[2] = (lpfX[i+1]-lpfX[i])/(6*nStep);
				*lpfCi = (float)(lpcYi[i+1]-lpcYi[i])/nStep - lpfCi[1]*nStep - lpfCi[2]*nStep*nStep;
			}
			//****** Last Point *****
			lpfCi[1] = (float)0;
			lpfCi[2] = (float)0;
			*lpfCi = (float)0;
			lpfA=(float far*)GlobalReAllocPtr(lpfA,sizeof(float)*nN*3,GMEM_MOVEABLE);
			lpfCi = lpfA;
		}
		//************** Spline Section,Equal And Non Equal, int type ************************
		//******** If Equal Step, Then nStep Is Just Step. 
		//******** Else, lpfStepi Is Step Matrix And -nStep Is Quantity of Points**** 
		INTERPOLATECLASS(int nX0_,int nXn_,int nStep_,float far *lpfYi,float far *lpfStepi){
			lpfY=lpfYi;
			lpfS=lpfStepi;
			nX0 = nX0_,nXn=nXn_,nStep=nStep_;

			PointerInit();
			if(nStep > 0 )
				nN=(nXn-nX0)/nStep+1; 
			else
				nN=-nStep;
            //*********** Matrix For Computation *******************			
			lpfA=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN*nN );
			lpfX=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN );
			lpfB=(float far*)GlobalAllocPtr(GHND,sizeof(float)*nN );
			if( !lpfA || !lpfX || !lpfB ){
				MessageBox(GetFocus(),"Cann't Allocate Memory For Interpolation","Get Interpolation",MB_OK);
				PostQuitMessage(-1);
			}
			//****** Make Matrix A and B ************
			int i;
			float far *lpfABak;
				// *** Boundary Condition ******
			*lpfA = (float)2; lpfA[1]=(float).5;
			if(nStep > 0 )
				*lpfB = (float)(6./(2*nStep)*( (float)(lpfYi[1]-(*lpfYi))/ nStep ));
			else
				*lpfB = (float)(6./(2*lpfStepi[0])*( (float)(lpfYi[1]-(*lpfYi))/ (*lpfStepi) ));

				// *** Normal Condition ******
			for(i=1,lpfABak=lpfA+nN;i<nN-1;i++,lpfABak+=nN+1){
				if(nStep > 0 ){
					lpfABak[1] = (float)2;//*** Constant ...***
					lpfABak[2] = (float).5;//*** Ramda,Equal Step...***
					*lpfABak = (float).5;//*** Mu,Equal Step...***
					lpfB[i] = (float)(6./(2*nStep)*( (float)(lpfYi[i+1]-lpfYi[i])/ nStep 
										-(float)(lpfYi[i]-lpfYi[i-1])/ nStep));
				}
				else{
					lpfABak[1] = (float)2;//*** Constant ...***
					lpfABak[2] = lpfStepi[i]/(lpfStepi[i-1]+lpfStepi[i]);//*** Ramda,
					*lpfABak = 1-lpfABak[2];//*** Mu,=1-Ramda_i
					lpfB[i] = (float)(6./(lpfStepi[i-1]+lpfStepi[i])
						*( (float)(lpfYi[i+1]-lpfYi[i])/ lpfStepi[i] 
						-(float)(lpfYi[i]-lpfYi[i-1])/ lpfStepi[i-1]));
				}
			}
				 // *** Boundary Condition ******
			*lpfABak = (float).5;//*** Mu,Equal Step...***
			lpfABak[1] = (float)2;//*** Constant ...***
			if(nStep > 0 )
				lpfB[i] = (float)(6./(2*nStep)*( -(float)(lpfYi[i]-lpfYi[i-1])/ nStep ));
			else 
				lpfB[i] = (float)(6./(2*lpfYi[i-1])*( -(float)(lpfYi[i]-lpfYi[i-1])/ lpfYi[i-1] ));
			//******* Comput Matrix X ************
			if(!MatrixOperation((float *)lpfA,nN,nN,(float *)lpfX,1,nN,(float *)lpfB,'1',FALSE)){
				MessageBox(GetFocus(),"Interpolation Error! Result Is Unreliable!","Spline Interpolation",MB_OK);
			}
			//******* Comput Efficient Values Into Matrix A ****
			for(i=0,lpfCi=lpfA; i<nN-1; i++,lpfCi += 3){
				lpfCi[1] = lpfX[i]/2;
				if(nStep > 0 ){
					lpfCi[2] = (lpfX[i+1]-lpfX[i])/(6*nStep);
					*lpfCi = (float)(lpfYi[i+1]-lpfYi[i])/nStep - lpfCi[1]*nStep - lpfCi[2]*nStep*nStep;
				}
				else{
					lpfCi[2] = (lpfX[i+1]-lpfX[i])/(6*lpfStepi[i]);
					*lpfCi = (float)(lpfYi[i+1]-lpfYi[i])/lpfStepi[i] - lpfCi[1]*lpfStepi[i] - lpfCi[2]*lpfStepi[i]*lpfStepi[i];
				}
			}
			//****** Last Point *****
			lpfCi[1] = (float)0;
			lpfCi[2] = (float)0;
			*lpfCi = (float)0;
			lpfA=(float far*)GlobalReAllocPtr(lpfA,sizeof(float)*nN*3,GMEM_MOVEABLE);
			lpfCi = lpfA;
		}
		~INTERPOLATECLASS(){
			if(lpfDivisor)	GlobalFreePtr(lpfDivisor); 
			if(lpfA )	GlobalFreePtr(lpfA); 
			if(lpfX )	GlobalFreePtr(lpfX); 
			if(lpfB )	GlobalFreePtr(lpfB); 
		}
		void PointerInit(){
			lpfDivisor = NULL;
			lpfA = NULL;
			lpfX = NULL;
			lpfB = NULL;
		}
		 float GetLagrangueInterpolate_EqualInterval(int nX0,int nXn,int nStep,float fX,char far *lpcYi);		
		 float GetSplineInterpolate_EqualInterval(float fX);		
		 float GetSplineInterpolate_NonEqualInterval_Pluse(float fX);
};

template <class T_HoughTable>void GetHoughEfficients(T_HoughTable *lpHoughTbl,unsigned short *lpuEfficient,BYTE *bpS,
						int nSizeX,int nSizeY,int nThiltaMax,int nRoMax){
	for(int j=0;j<nSizeY;j++){
		for(int k=0;k<nSizeX;k++,bpS++){
			if(*bpS){
				// Get Hough Efficients...
				for(int nThilta=0;nThilta<nThiltaMax;nThilta++){
					int nRo=lpHoughTbl[nThilta*nSizeX*nSizeY+j*nSizeX+k];
						lpuEfficient[nRo*nThiltaMax+nThilta] += (*bpS);
				}
			}
		}
	}
	// Cut off ...
	unsigned short uMax=0;
	for(int nRo=0;nRo<nRoMax;nRo++){
		for(int nThilta=0;nThilta<nThiltaMax;nThilta++)
			if(uMax < lpuEfficient[nRo*nThiltaMax+nThilta])
				uMax = lpuEfficient[nRo*nThiltaMax+nThilta];
	}
#define EDG_PERCENT 0.7f

	uMax *= EDG_PERCENT;
	for(nRo=0;nRo<nRoMax;nRo++){
		for(int nThilta=0;nThilta<nThiltaMax;nThilta++)
			if( uMax > lpuEfficient[nRo*nThiltaMax+nThilta])
				lpuEfficient[nRo*nThiltaMax+nThilta] = 0;
	}
}

template <class TEigenSrc,class TEigenVect,class TEigenVal> 
	BOOL GetEigenFromSymetric(TEigenSrc *lpSrc,TEigenVect *lpVect,TEigenVal *lpVal,int nRowCol,int nMaxRepeat,float fErr,BOOL bWait,BOOL bMedSaveRequire){
	// Make Initial Eigenvect to Unity Matrix
	int i,j;
	TEigenVect *lpTempVect;
	lpTempVect = lpVect;

	if(bMedSaveRequire && (IDYES == ::MessageBox(::GetFocus(),"Load Median Result?","Get Eigens",MB_YESNO))){
#ifdef	_DEBUG
//		CFile fileMed("C:\\Med.Eig",CFile::modeRead);
//		fileMed.Read(lpSrc,nRowCol*nRowCol*sizeof(TEigenSrc));
//		fileMed.Read(lpVect,nRowCol*nRowCol*sizeof(TEigenVect));
//		fileMed.Read(lpVal,nRowCol*sizeof(TEigenVal));
#endif

	}
	else{
		for(j=0;j<nRowCol;j++)
			for(i=0;i<nRowCol;i++,lpTempVect++){
				if(i==j)
					*lpTempVect=1;
				else
					*lpTempVect=0;
			}
	}

	CWaitDlg wait;
	if(bWait){
		wait.Create(CWaitDlg::IDD);
		wait.SetWindowText("Get EigenVector...");
	}
	int nCurRep=nMaxRepeat;
	while(nCurRep--){
		if(bMedSaveRequire){
#ifdef	_DEBUG
//			CFile fileMed("\\Med.Eig",CFile::modeCreate | CFile::modeWrite);
//			fileMed.Write(lpSrc,nRowCol*nRowCol*sizeof(TEigenSrc));
//			fileMed.Write(lpVect,nRowCol*nRowCol*sizeof(TEigenVect));
//			fileMed.Write(lpVal,nRowCol*sizeof(TEigenVal));
#endif
		}
		if(bWait){
			// Wait Status...
			wait.SetDlgItemInt(ID_WAITTEXT,(nMaxRepeat-nCurRep)*100/nMaxRepeat);
		}
		// Check if Precise Result is obtained
		TEigenSrc *lpTempSrc;
		lpTempSrc = lpSrc;
		for(j=0;j<nRowCol-1;j++){
			for(i=j+1,lpTempSrc += i;i<nRowCol;i++,lpTempSrc++){
				if(fabs((double)(*lpTempSrc)) > fErr)	// Yet Not precise!
					break;
			}
			if(i < nRowCol)	// Yet Not precise!
				break;
		}
		if(j == nRowCol-1 && i == nRowCol){	// Reached To precise!
			for(i=0;i<nRowCol;i++,lpVal++,lpSrc+=nRowCol,lpSrc++)
				*lpVal = *lpSrc;
			return TRUE;
		}


		int p,q;
		lpTempSrc = lpSrc;
		CWaitDlg waitSubLoop;
		if(bWait){
			waitSubLoop.Create(CWaitDlg::IDD);
			waitSubLoop.SetWindowText("Sub Loop...");
		}
		for(q=0;q<nRowCol-1;q++){
			if(bWait){
				// Wait Status...
				waitSubLoop.SetDlgItemInt(ID_WAITTEXT,q*100/nRowCol);

				MSG msg;
				while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){
					if(!wait.IsDialogMessage(&msg)){
						if(!waitSubLoop.IsDialogMessage(&msg)){
							if(!AfxGetMainWnd()->PreTranslateMessage(&msg)){
								TranslateMessage(&msg);
								::SendMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
							}
						}
					}
				}
				if(wait.bStop)
					break;
			}
			for(p=q+1,lpTempSrc += p;p<nRowCol;p++,lpTempSrc++){
				// Find Element > fErr
				if(fabs((double)(*lpTempSrc)) <= fErr)
					continue;
				// Comput cot(2x),sin(x),cos(x)
				TEigenSrc *lpSrc_p_p=&lpSrc[p*nRowCol+p];
				TEigenSrc *lpSrc_q_q=&lpSrc[q*nRowCol+q];
				TEigenSrc *lpSrc_q_p=&lpSrc[q*nRowCol+p];
				double dblCot2X = (*lpSrc_p_p - *lpSrc_q_q)/(*lpTempSrc)/2.0f;

				int nSgn=1;
				if(dblCot2X < 0.0f)
					nSgn=-1;
				double dblCotX=dblCot2X+nSgn*sqrt(1+dblCot2X*dblCot2X);
				double dblSinX=nSgn/sqrt(1+dblCotX*dblCotX);
				double dblCosX=dblSinX*dblCotX;
				int nPos_j_p = p;
				int nPos_j_q = q;
				for(j=0;j<nRowCol;j++,nPos_j_p+=nRowCol,nPos_j_q+=nRowCol){
					TEigenVect dblBT=lpVect[nPos_j_q];
					lpVect[nPos_j_q]=dblBT*dblCosX-lpVect[nPos_j_p]*dblSinX;
					lpVect[nPos_j_p]=dblBT*dblSinX+lpVect[nPos_j_p]*dblCosX;
					if(j==p || j==q)
						continue;
					// comput a(ip),a(iq)
					TEigenSrc dblAT=lpSrc[nPos_j_q];
					lpSrc[nPos_j_q]=dblAT*dblCosX-lpSrc[nPos_j_p]*dblSinX;
					lpSrc[nPos_j_p]=dblAT*dblSinX+lpSrc[nPos_j_p]*dblCosX;
				}
				// comput a(pp),a(qq),a(pq),a(qp)
				double dblXT = 2*(*lpTempSrc)*dblSinX*dblCosX;
				TEigenSrc dblAT = *lpSrc_q_q;
				TEigenSrc dblBT = *lpSrc_p_p;
				*lpSrc_q_q = dblAT*dblCosX*dblCosX+dblBT*dblSinX*dblSinX-dblXT;
				*lpSrc_p_p = dblAT*dblSinX*dblSinX+dblBT*dblCosX*dblCosX+dblXT;
				*lpSrc_q_p = *lpSrc_q_p*(dblCosX*dblCosX-dblSinX*dblSinX)+(dblAT-dblBT)*dblSinX*dblCosX;
				lpSrc[p*nRowCol+q]= *lpSrc_q_p;

				TEigenSrc *lpSrc_q_i = &lpSrc[q*nRowCol];
				TEigenSrc *lpSrc_p_i = &lpSrc[p*nRowCol];
				TEigenSrc *lpSrc_i_q = &lpSrc[q];
				TEigenSrc *lpSrc_i_p = &lpSrc[p];
				for(i=0;i<nRowCol;i++,lpSrc_q_i++,lpSrc_p_i++,lpSrc_i_q+=nRowCol,lpSrc_i_p+=nRowCol){
					*lpSrc_q_i = *lpSrc_i_q;
					*lpSrc_p_i = *lpSrc_i_p;
				}
			}
		}
		if(bWait)
			waitSubLoop.DestroyWindow();
	}
	if(bWait)
		wait.DestroyWindow();
	// Not Reached in repeatition given
	for(i=0;i<nRowCol;i++,lpVal++,lpSrc+=nRowCol,lpSrc++)
		*lpVal = *lpSrc;
	return FALSE;
}

template <class T_Vect1,class T_Vect2,class T_Weig,class T_Result> void PowNorm(T_Vect1 *fpV1,T_Vect2 *fpV2,int nDimOfVect,T_Weig *fpW,T_Result &fDelt){
	fDelt=0;
	// Eucledian
	if(!fpW){
		while(nDimOfVect--){
			T_Result fMed=(*fpV2) - (*fpV1);
			fDelt += fMed*fMed;
			// Attention !! if T_Result is (int), then use abs!
	//		fDelt += fabs(fMed);
			fpV1++,fpV2++;
		}
	}
	else{
		while(nDimOfVect--){
			T_Result fMed=(*fpV2) - (*fpV1);
			fDelt += fMed*fMed/(*fpW);
			// Attention !! if T_Result is (int), then use abs!
	//		fDelt += fabs(fMed);
			fpV1++,fpV2++,fpW++;
		}
	}
}

// ExchangeSort: The ExchangeSort compares each element--starting with
// the first--with every following element. If any of the following
// elements is smaller than the current element, it is exchanged with
// the current element and the process is repeated for the next element.
//
template <class Ttype,class TtypeSlav> void ExchangeSort(Ttype *pData,TtypeSlav *pslav,int nN,BOOL bIncrease){
	int nCur,nMinMax,nNext;

    for( nCur = 0; nCur < nN; nCur++ )
    {
        nMinMax = nCur;
        for( nNext = nCur; nNext < nN; nNext++ ){
			if(bIncrease){
	            if( pData[nNext] < pData[nMinMax] )
		            nMinMax = nNext;
			}
			else{
	            if( pData[nNext] > pData[nMinMax] )
		            nMinMax = nNext;
			}
        }
        // If a row is shorter than the current row, swap those two
        // array elements.
        if( nMinMax > nCur ){
    		Ttype fTmp;
		    fTmp = pData[nCur];
		    pData[nCur] = pData[nMinMax];
		    pData[nMinMax] = fTmp;

			if(pslav){
				TtypeSlav slavMed;
			    slavMed = pslav[nCur];
			    pslav[nCur] = pslav[nMinMax];
			    pslav[nMinMax] = slavMed;
			}
        }
    }
}


// ISODATA.....


template <class T_srcX,class T_sortU,class T_ceedV>float ComputSortMatrixU(T_srcX *fpX,T_sortU *fpU,T_ceedV *fpV,int nDimOfVect,int nNumOfVect,int nNumOfCeed,float fFuzzyness,BOOL bWait){
/*
	CWaitDlg waitSub;
	if(bWait){
		waitSub.Create(CWaitDlg::IDD);
		waitSub.SetWindowText("U-Loop...");
	}
*/
	float EpsBak=0;

	T_sortU *fpTempU=fpU;
	T_ceedV *fpTempV=fpV;
	for(int i=0;i<nNumOfCeed;i++,fpTempV+=nDimOfVect){

/*
		// Pickmessage for Waiting
		if(bWait){
			// Wait Status...
			waitSub.SetDlgItemInt(ID_WAITTEXT,i*100/nNumOfCeed);

			MSG msg;
			while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){
				if(!waitSub.IsDialogMessage(&msg)){
					if(!AfxGetMainWnd()->PreTranslateMessage(&msg)){
						TranslateMessage(&msg);
						::SendMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
					}
				}
			}
			if(waitSub.bStop)
				break;
		}
*/

		T_srcX *fpTempX=fpX;
		for(int j=0;j<nNumOfVect;j++,fpTempU++,fpTempX+=nDimOfVect){
		//+++++++ Stube FuncExp_1_U_D(X,j,V,i)++++++++++++++
			double dblSum=0;
			float Delt1;
			PowNorm(fpTempV,fpTempX,nDimOfVect,(float*)NULL,Delt1);
			T_ceedV *fpTempV_=fpV;
			float Ubak;
			for(int l=0;l<nNumOfCeed;l++,fpTempV_+=nDimOfVect){
				float Delt2;
				PowNorm(fpTempV_,fpTempX,nDimOfVect,(float*)NULL,Delt2);
				if( Delt2 == 0 ) {
					if( Delt1 == 0 )
						dblSum += 1;
					else{
						Ubak = 0;
						goto EX_FORLOOP;
					}
				}
				else 
					dblSum += pow( Delt1/Delt2, 1/(fFuzzyness-1) );
			}
	
			Ubak =(1./dblSum); 
EX_FORLOOP:						
		//*++++++++++++++End Cond Check+++++++++++++++++
			if( fabs (*fpTempU - Ubak) > EpsBak )
				EpsBak = fabs (*fpTempU - Ubak);
			*fpTempU = Ubak;
		}
	}
//	if(bWait)
//		waitSub.DestroyWindow();
	return EpsBak;
}

template <class T_srcX,class T_sortU,class T_ceedV>void ISODATA_Group(T_srcX *fpX,T_sortU *fpU,T_ceedV *fpV,int nDimOfVect,int nNumOfVect,int nNumOfCeed,float fFuzzyness,float fAcuracy,BOOL bWait){
	
	float EpsBak=100.0f;
	double dblSDiv,dblBak;
	int i,j,k;
	CMemAllocFree memSum(GHND,nDimOfVect*sizeof(double));
	
	
	int RepNum=0;

	//---------Optional Set Of Ini-U-Matrix-----
	T_sortU *fpTempU=fpU;
	for(i=0;i<nNumOfCeed;i++){
		for(j=0;j<nNumOfVect;j++,fpTempU++){
			if(j % nNumOfCeed == i)
				*fpTempU = 1.0f;
			else 
				*fpTempU = 0.0f;
		}
	}
/*
	if( Cmd_Anly( 'a' ) ){ 
		Set_Auto_nNumOfCeed();
		goto U_Comput;
	}
*/
	CWaitDlg wait;
	if(bWait){
		wait.Create(CWaitDlg::IDD);
		wait.SetWindowText("ISODATA...");
	}
Rep:

	if(bWait){
		// Wait Status...
		wait.SetDlgItemInt(ID_WAITTEXT,(int)(fAcuracy*100/EpsBak));
	}
	
	RepNum++;

	//--------V (Centre) Computation-----------
	fpTempU=fpU;
	T_ceedV *fpTempV=fpV;
	for(i=0;i<nNumOfCeed;i++){

		// Pickmessage for Waiting
		if(bWait){
			MSG msg;
			while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){
				if(!wait.IsDialogMessage(&msg)){
					if(!AfxGetMainWnd()->PreTranslateMessage(&msg)){
						TranslateMessage(&msg);
						::SendMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
					}
				}
			}
			if(wait.bStop)
				break;
		}


		//*+++++++stub	FuncExp_2_U_D( V[i],i,U[i],X[0]) +++++
		memset(memSum.lpData,0,nDimOfVect*sizeof(double));
		dblSDiv=0;
		T_srcX *fpTempX=fpX;
		for(j=0;j<nNumOfVect;j++,fpTempU++){
			dblBak = pow( *fpTempU,fFuzzyness);
			dblSDiv += dblBak ;
			double *dblpSum=(double*)memSum.lpData;
			for(k=0;k<nDimOfVect;k++,dblpSum++,fpTempX++){
				*dblpSum += dblBak * (*fpTempX);
			}
		}
		double *dblpSum=(double*)memSum.lpData;
		for(k=0;k<nDimOfVect;k++,fpTempV++,dblpSum++){
			(*fpTempV)=(*dblpSum)/dblSDiv;
		}
		//+++++++++++++++++++++++++++++++++++++++
	}
	//--------U (Sort Matrix) Comput & End-Cond Check------
U_Comput:	
	EpsBak = ComputSortMatrixU(fpX,fpU,fpV,nDimOfVect,nNumOfVect,nNumOfCeed,fFuzzyness,bWait);
	
	if(	EpsBak >= fAcuracy )
		goto Rep; 		

	if(bWait)
		wait.DestroyWindow();
/*
	//--- Minimum Distance Between nNumOfCeed------
	float MinDist=1000;
	for(i=0;i<nNumOfCeed-1;i++){
		for(j1=i+1;j1<nNumOfCeed;j1++){
			Delt1 = PowNorm(&fpV[j1*nNumOfVect],&fpV[i*nNumOfVect],nDimOfVect);
			if(Delt1 < MinDist)
				MinDist=Delt1;
		}
	}
*/

}

#ifndef IDD_WAIT
#define IDD_WAIT 101
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompWaitDlg dialog

 class CCompWaitDlg : public CDialog
{
// Construction
public:
	 CCompWaitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompWaitDlg)
	enum { IDD = IDD_WAIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompWaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
//variables
private:
	HCURSOR hOldCursor;
	int nPercent;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompWaitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeWaittext();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
