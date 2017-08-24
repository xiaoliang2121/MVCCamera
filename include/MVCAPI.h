 #ifndef _MVC1000APIUSER_H
#define _MVC1000APIUSER_H

#include "CommonDataType.h"

#ifdef MVC1000API_EXPORTS
#define MV_API extern __declspec(dllexport) MV_RETURN_CODE WINAPI
#else
#define MV_API extern __declspec(dllimport) MV_RETURN_CODE WINAPI
#endif

#ifdef __cplusplus
extern "C"{
#else
#endif


/*--------------------------------------------------------------
	initial device
  --------------------------------------------------------------*/
MV_API MV_Usb2Init(	IN LPCSTR pFilterName, IN int*  index,	
				    IN struct CapInfoStruct *pCapInfo, OUT HANDLE* hImager);

/*--------------------------------------------------------------
	uninitial device
  --------------------------------------------------------------*/
MV_API MV_Usb2Uninit(IN HANDLE* hImager);


/*--------------------------------------------------------------
	get frame rate (frames/second)
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetNumberDevices(IN HANDLE hImager,	
						OUT PU32 pNumberDevices);


/*--------------------------------------------------------------
	start preview
  --------------------------------------------------------------*/
MV_API
MV_Usb2StartCapture(IN HANDLE hImager, BOOL bCapture);

MV_API 
MV_Usb2Start(IN HANDLE hImager,	
			 LPCTSTR   title,								//="My View"
			 ULONG     style,								//=WS_OVERLAPPEDWINDOW|WS_VISIBLE
			 ULONG     x,									//=0
			 ULONG     y,									//=0
			 ULONG     width,								//=-1
			 ULONG     height,								//=-1
			 HWND      parent,								//=NULL
			 ULONG     nId,									//=0
			 IN int    ViewDataThreadPriority,				//=THREAD_PRIORITY_NORMAL
			 IN int    ViewDrawThreadPriority);				 //=THREAD_PRIORITY_NORMAL


/*--------------------------------------------------------------
	pause preview
  --------------------------------------------------------------*/
MV_API
MV_Usb2PausePreview(IN HANDLE hImager,	IN ULONG pause);

MV_API
MV_Usb2Pause(IN HANDLE hImager,	IN ULONG pause);

/*--------------------------------------------------------------
	stop preview
  --------------------------------------------------------------*/
MV_API
MV_Usb2Stop(IN HANDLE hImager);			// Imager object handle


/*--------------------------------------------------------------
	get frame rate
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetFrameRate(IN HANDLE hImager,	
					OUT float *pfFrameRate);

/*--------------------------------------------------------------
	set preivew window's size
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetViewWin(IN HANDLE hImager, 
				  IN PMVRECT pRect);	


/*--------------------------------------------------------------
	get preview window's size
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetViewWin( IN  HANDLE   hImager,  
				   OUT PMVRECT pRect);

/*--------------------------------------------------------------
	set preview window same to capture window size
  --------------------------------------------------------------*/
MV_API
MV_Usb2ResetViewWin(IN HANDLE hImager);

/*--------------------------------------------------------------
	set capture and preview property
  --------------------------------------------------------------*/
MV_API  
MV_Usb2SetCapInfo(IN HANDLE hImager, 
				  struct CapInfoStruct *pCapInfo);

/*--------------------------------------------------------------
	set capture and preview property
  --------------------------------------------------------------*/
MV_API 
MV_Usb2SetPartOfCapInfo(IN HANDLE hImager, 
						IN struct CapInfoStruct *pCapInfo);

/*--------------------------------------------------------------
	get 24bit rgb data of frame
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetRgbData(IN HANDLE hImager, 
				  IN struct CapInfoStruct *pCapInfo, ///////change
				  OUT BYTE* pDest);

/*--------------------------------------------------------------
	get the raw data of one frame
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetRawData(IN HANDLE hImager, IN struct CapInfoStruct *pCapInfo);

/*--------------------------------------------------------------
	convert 8bit raw to 24bit rgb 
  --------------------------------------------------------------*/
MV_API
MV_Usb2ConvertRawToRgb(IN HANDLE   hImager,
					   IN BYTE*    pSrc,		// Ptr to the source buffer
					   IN ULONG    width,		//image width
					   IN ULONG    height,		//image height
					   OUT BYTE*   pDest);

/*--------------------------------------------------------------
	capture one frame and save to bmp file
  --------------------------------------------------------------*/
MV_API
MV_Usb2SaveFrameAsBmp(IN HANDLE   hImager, 
					  IN struct   CapInfoStruct *pCapInfo, 
					  OUT BYTE    *pDest, 
					  IN LPCTSTR  FileName);

/*--------------------------------------------------------------
	auto exposure
  --------------------------------------------------------------*/
MV_API 
MV_Usb2AutoExposure(IN HANDLE  hImager, 
					IN BOOL    bAdjustExp, 
					IN BYTE    btTarget, 
					IN LPVOID  lpFunc, 
					IN LONG    *pParam);

/*--------------------------------------------------------------
	auto white balance
  --------------------------------------------------------------*/
MV_API 
MV_Usb2AWB(IN HANDLE   hImager,	
		   IN BOOL     bDoAgc, 
		   IN BYTE     btRTarget, 
		   IN BYTE     btGTarget, 
		   IN BYTE     btBTarget, 
		   IN LPVOID   lpFunc, 
		   IN LONG     *pParam);

/*--------------------------------------------------------------
	set selectable region AE'S callback function
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetAeCallBackFunction(IN HANDLE hImager,
							 IN BYTE   btAeTarget, 
							 IN LPVOID lpFunc, 
							 IN LONG * pParam);

/*--------------------------------------------------------------
	set selectable region AWB'S callback function
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetAwbCallBackFunction(IN HANDLE hImager,
							  IN BYTE   btRTarget,
							  IN BYTE   btGTarget,
							  IN BYTE   btBTarget, 
							  IN LPVOID lpFunc, 
							  IN LONG   *pParam);

/*--------------------------------------------------------------
	set GDI MASK'S callback function
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetMaskCallBack(IN HANDLE hImager, LPMV_CALLBACK2 MaskCallBackFunction, LPVOID lpUser);

/*--------------------------------------------------------------
	if GDI MASK
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetMask(IN HANDLE hImager,BOOL bMask);

/*--------------------------------------------------------------
	load gammatable
  --------------------------------------------------------------*/
MV_API 
MV_Usb2SetGamma(IN HANDLE hImager, 
				IN BYTE   *pGamma, 
				IN BOOL   bGamma);

/*--------------------------------------------------------------
	set gamma value to correlation, (0~2)
  --------------------------------------------------------------*/
MV_API 
MV_Usb2SetGammaValue(HANDLE hImager, 
					 float  GammaValue,  
					 BOOL   bGammaOn);

/*--------------------------------------------------------------
	B/W preview
  --------------------------------------------------------------*/
MV_API 
MV_Usb2SetBw(IN HANDLE hImager,	
			 IN BOOL   bBw);

/*--------------------------------------------------------------
	clear view function
  --------------------------------------------------------------*/
MV_API
MV_Usb2ClearView(IN HANDLE hImager,IN BOOL bClearView);

MV_API
/*--------------------------------------------------------------
	set image mirror mode
  --------------------------------------------------------------*/
MV_Usb2SetMirrorMode( IN HANDLE hImager, BYTE nMode );

/*--------------------------------------------------------------------------
	detect Noise Pixel.
  --------------------------------------------------------------*/
MV_API
MV_Usb2DetectNoisePixel( IN HANDLE hImager );


/*--------------------------------------------------------------------------
	removal Noise Pixel. first, you should do detect noise pixels
  --------------------------------------------------------------*/
MV_API
MV_Usb2RemoveNoisePixel( IN HANDLE hImager, BOOL bRemoveNoise );

MV_API
MV_Usb2Enhanced( IN HANDLE hImager, BOOL bEnhance );

/*--------------------------------------------------------------
	set frame (frame arrive) callback function. when one frame finished transfer,
	callback function will be called
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetFrameCallBack( IN HANDLE hImager, LPMV_CALLBACK2 FrameCallBackFunction, LPVOID lpUser);

MV_API
MV_Usb2SetRawCallBack( IN HANDLE hImager, LPMV_CALLBACK2 RawCallBackFunction, LPVOID lpUser);

MV_API
MV_Usb2SetFrameArriveCallBack(
							  IN HANDLE hImager, 
							  LPMV_CALLBACK fpFrameArriveCallBackFunction, 
							  LPVOID lpUser
							  );

/*--------------------------------------------------------------------------
	real time image process.
  --------------------------------------------------------------*/
MV_API
MV_Usb2SharpPreview( IN HANDLE hImager, BOOL bSharp );


/*--------------------------------------------------------------------------
	real time image process.
  --------------------------------------------------------------*/
MV_API
MV_Usb2MeanPreview( IN HANDLE hImager, BOOL bMean );

/*--------------------------------------------------------------------------
	real time image process.
  --------------------------------------------------------------*/
MV_API
MV_Usb2EmbossPreview(IN HANDLE hImager, BOOL bEmboss);


/*--------------------------------------------------------------------------
	real time image process.
  --------------------------------------------------------------*/
MV_API
MV_Usb2EdgeDetectPreview(IN HANDLE hImager, BOOL bEdgeDetect);

/*--------------------------------------------------------------------------
	real time image process.
  --------------------------------------------------------------*/
MV_API
MV_Usb2GaussBlurPreview(IN HANDLE hImager, BOOL bGaussBlur);

/*--------------------------------------------------------------
	get serial no. no need call for now.
  --------------------------------------------------------------*/
MV_API	
MV_Usb2GetSerial(IN HANDLE hImager, OUT LPTSTR szSerial);

/*--------------------------------------------------------------
	get verification key. no need call for now.
  --------------------------------------------------------------*/
MV_API
MV_Usb2ReadKey( IN HANDLE hImager );

/*--------------------------------------------------------------
	get device type. the definition of device type refers to "commondatatype.h".
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetDeviceType( IN HANDLE hImager, OUT USHORT *pSubNum );

/*---------------------------------------------------------------------------------
	Set device operation mode, continue mode or trigger mode.
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetOpMode( IN HANDLE hImager, BYTE nMode, BOOL bStrobe );

/*---------------------------------------------------------------------------------
	For trigger mode, set the trigger pulse width. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetTriggerPulse( IN HANDLE hImager, WORD nPulseWidth );

/*---------------------------------------------------------------------------------
	For trigger mode, set the trigger delay. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetTriggerDelay( IN HANDLE hImager,WORD nTrgDelay);

/*---------------------------------------------------------------------------------
	For trigger mode, set the trigger delay. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetTriggerInterval( IN HANDLE hImager,WORD nTrgInterval);

/*---------------------------------------------------------------------------------
	For continue mode, set the led width. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetLedWidth( IN HANDLE hImager,WORD nLedWidth);

/*---------------------------------------------------------------------------------
	For continue mode, set the led delay. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetLedDelay( IN HANDLE hImager,WORD nLedDelay);

/*---------------------------------------------------------------------------------
	For trigger mode, send a trigger to camera by software to capture one frame. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SendUserTrigger( IN HANDLE hImager );

/*---------------------------------------------------------------------------------
	For trigger mode, set the polarity of trigger and strobe signal. 
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetSignalPolarity( IN HANDLE hImager, BYTE nTrigglePolarity, BYTE nStrobePolarity);

/*---------------------------------------------------------------------------------
	Set A/D mode of the camera.         
	//0 - 5M - [11:4], 1M/3M - [9:2]
    //1 - 5M - [10:3], 1M/3M - [8:1]
    //2 - 5M - [9:2],  1M/3M - [7:0]
    //3 - 5M - [8:1],  1M/3M - [9:2]
    //4 - 5M - [7:0],  1M/3M - [9:2]
    //else - 5M - [11:4], 1M/3M - [9:2]
  --------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetADCMode( IN HANDLE hImager, BYTE nMode );

MV_API
MV_Usb2SetTestPattern(IN HANDLE hImager, BYTE nMode);

MV_API
MV_Usb2SetAgcAec( IN HANDLE hImager, BYTE nMode );

MV_API
MV_Usb2SetAWBProcess(IN HANDLE hImager, BOOL bAWBProcess, float *pRRatio, float *pBRatio);

MV_API
MV_Usb2SetRadio( IN HANDLE hImager, float fRate, BYTE nTable );

MV_API
MV_Usb2GetDeviceSubNum( IN HANDLE hImager, OUT USHORT *pSubNum );

/*--------------------------------------------------------------
	Reserved,Can not used.
	Color enhance
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetImageProcess(IN HANDLE hImager, int nBrightness, int nContrast, int nHue, int nSaturation, BOOL bProcess);

/*--------------------------------------------------------------
	Save user data to eeprom
  --------------------------------------------------------------*/
MV_API
MV_Usb2SaveUserData( IN HANDLE hImager, MVUserData * pUserData );

/*--------------------------------------------------------------
	Load user data from eeprom
  --------------------------------------------------------------*/
MV_API
MV_Usb2LoadUserData( IN HANDLE hImager, MVUserData * pUserData );

/*-------------------------------------------------------------------------------------------
	Trigger mode, the number of frame generated by one trigger.  For CCD Camera.
  -------------------------------------------------------------------------------------------*/
MV_API
MV_Usb2SetTriggerMode( IN HANDLE hImager, BYTE nMode );

/*--------------------------------------------------------------
	Flash mode. For CCD Camera.
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetFlashMode( IN HANDLE hImager, BYTE nMode );

/*--------------------------------------------------------------
	long time integration. For CCD Camera.
  --------------------------------------------------------------*/
MV_API
MV_Usb2LongTimeIntegration( IN HANDLE hImager, DWORD nIntTimeL, DWORD nIntTimeH );

/*--------------------------------------------------------------
	Set Display Threshold. For 14bit CCD Camera MVC1450DMF14.
  --------------------------------------------------------------*/
MV_API
MV_Usb2SetDataThreshold( IN HANDLE hImager, UINT nMaxValue, UINT nMinValue );

/*--------------------------------------------------------------
	Get or set temperature for 14bit refraction CCD.
  --------------------------------------------------------------*/
MV_API
MV_Usb2GetTemperature( IN HANDLE hImager, float *pTemperature );

MV_API
MV_Usb2SetTemperature( IN HANDLE hImager, BOOL bTemperature, INT nTemperature );

/*--------------------------------------------------------------
Set AD CDS Gain, VGA Gain, Clamp for 14bit CCD.
--------------------------------------------------------------*/
MV_API
MV_Usb2SetCDSGain( IN HANDLE hImager, UINT nCDSGian );

MV_API
MV_Usb2SetADClamp( IN HANDLE hImager, UINT nClamp );

MV_API
MV_Usb2SetVGAGain( IN HANDLE hImager, UINT nVGAGian );

MV_API
MV_Usb2SetFilterProcess( IN HANDLE hImager, BOOL bFilter, UINT nFNum );

MV_API
MV_Usb2SetThreadAffinityMask( IN HANDLE hImager, DWORD nThreadAffinityMask );

MV_API
MV_Usb2SetTimingIntegration( IN HANDLE hImager, BYTE nFrameNo, DWORD nIntTimeL, DWORD nIntTimeH );

#ifdef __cplusplus
}
#endif

#endif //_MVC1000APIUSER_H

