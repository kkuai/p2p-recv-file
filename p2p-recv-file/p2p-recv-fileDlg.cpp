
// p2p-recv-fileDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "p2p-recv-file.h"
#include "p2p-recv-fileDlg.h"
#include "afxdialogex.h"
#include <sys/timeb.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int SendData(int fd, char* buff, int len) {
	int sended = 0;
	while (sended < len) {
		// 1 second timeout
		int wl = kkp2p_write(fd, buff + sended, len - sended, 1000);
		if (wl < 0) {
			printf("SendData error,fd:%d,ret:%d,len:%d,errno:%d,desc:%s.\n", fd, wl, len, errno, strerror(errno));
			return -1;
		}
		sended += wl;
	}
	return len;
}

int RecvData(int fd, char* buff, int len) {
	int recved = 0;
	while (recved < len) {
		// 1 second timeout
		int wl = kkp2p_read(fd, buff + recved, len - recved, 1000);
		if (wl < 0) {
			printf("RecvData error,fd:%d,ret:%d,len:%d,errno:%d,desc:%s.\n", fd, wl, len, errno, strerror(errno));
			return -1;
		}
		recved += wl;
	}
	return len;
}

bool IsHostLittleEndian() {
	static const int number = 1;
	return (1 == *(const char*)(&number));
}

uint64_t Ntohll(uint64_t val)
{
	if (IsHostLittleEndian()) {
		return (((uint64_t)ntohl((int32_t)((val << 32) >> 32))) << 32)
			| (uint32_t)ntohl((int32_t)(val >> 32));
	}
	else {
		return val;
	}
}

uint64_t Htonll(uint64_t val)
{
	if (IsHostLittleEndian()) {
		return (((uint64_t)htonl((int32_t)((val << 32) >> 32))) << 32)
			| (uint32_t)htonl((int32_t)(val >> 32));
	}
	else {
		return val;
	}
}

uint64_t TimeMilliSecond()
{
	timeb now;
	ftime(&now);
	return now.time * 1000 + now.millitm;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cp2precvfileDlg 对话框



Cp2precvfileDlg::Cp2precvfileDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_P2PRECVFILE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cp2precvfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_loginDomain);
	DDX_Control(pDX, IDC_EDIT1, m_loginPort);
	DDX_Control(pDX, IDC_EDIT5, m_connectType);
	DDX_Control(pDX, IDC_EDIT6, m_lanPort);
	DDX_Control(pDX, IDC_EDIT7, m_peerId);
	DDX_Control(pDX, IDC_EDIT4, m_peerKey);
	DDX_Control(pDX, IDC_BUTTON1, m_folderButton);
	DDX_Control(pDX, IDC_EDIT2, m_statInfo);
	DDX_Control(pDX, IDOK, m_startButton);
	DDX_Control(pDX, IDC_EDIT8, m_folderName);
}

BEGIN_MESSAGE_MAP(Cp2precvfileDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Cp2precvfileDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &Cp2precvfileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Cp2precvfileDlg 消息处理程序

BOOL Cp2precvfileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);
	//ShowWindow(SW_MINIMIZE);
	m_loginDomain.SetWindowText(_T("124.71.217.198"));
	m_loginPort.SetWindowText(_T("3080"));
	m_lanPort.SetWindowText(_T("3549"));
	m_peerId.SetWindowText(_T("kkuai-ipc-00001"));
	m_peerKey.SetWindowText(_T("WtXmjG"));

	m_connectType.EnableWindow(FALSE);
	m_statInfo.EnableWindow(FALSE);

	return TRUE;
}

void Cp2precvfileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cp2precvfileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cp2precvfileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT ThreadFun(LPVOID pParam)
{
	Cp2precvfileDlg*  p_Dlg = (Cp2precvfileDlg*)pParam;
	p_Dlg->BeginWork();
	return 0;
}

void Cp2precvfileDlg::OnBnClickedButton1()
{
	char szPath[1024]; 
	ZeroMemory(szPath, sizeof(szPath));

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = TEXT("please select folder for receive file");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp && SHGetPathFromIDList(lp, szPath))
	{
		//AfxMessageBox(szPath); 
	}
	else
	{
		AfxMessageBox(TEXT("invalid folder,please select again"));
	}
	m_strFolder = szPath;
	m_folderName.SetWindowText(m_strFolder);
	UpdateData(TRUE);
}


void Cp2precvfileDlg::OnBnClickedOk()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_loginDomain.GetWindowText(m_strDomain);

	CString serverPort;
	m_loginPort.GetWindowText(serverPort);
	char *buf = (LPSTR)(LPCTSTR)serverPort;
	m_usLoginPort = atoi(buf);

	m_lanPort.GetWindowText(serverPort);
	buf = (LPSTR)(LPCTSTR)serverPort;
	m_usLanPort = atoi(buf);

	m_peerId.GetWindowText(m_strPeerId);
	m_peerKey.GetWindowText(m_strPeerKey);

	m_loginDomain.EnableWindow(FALSE);
	m_loginPort.EnableWindow(FALSE);
	m_lanPort.EnableWindow(FALSE);
	m_folderName.EnableWindow(FALSE);
	m_peerId.EnableWindow(FALSE);
	m_peerKey.EnableWindow(FALSE);
	m_folderButton.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	::AfxBeginThread(ThreadFun, this);
	return;
}

void Cp2precvfileDlg::BeginWork()
{
	// open file
	char * szDirName = (LPSTR)(LPCTSTR)m_strFolder;
	if (szDirName == NULL){
		AfxMessageBox(A2T("folder name is empty"));
		return;
	}
	memset(&m_engineConf, 0, sizeof(m_engineConf));
	m_engineConf.login_domain = m_strDomain.GetBuffer(0);
	m_engineConf.login_port = m_usLoginPort;
	m_engineConf.lan_search_port = m_usLanPort;
	m_engineConf.log_path = NULL;
	m_engineConf.max_log_size = 1024 * 1024;

	kkp2p_engine_t* kkp2p_engine = kkp2p_engine_init(&m_engineConf, 10000);
	if (kkp2p_engine == NULL) {
		AfxMessageBox(A2T("kkp2p_engine_init error"));
		return;
	}

	// error level
	kkp2p_switch_log_level(kkp2p_engine, 1);
	kkp2p_join_net(kkp2p_engine, m_strPeerId.GetBuffer(0), m_strPeerKey.GetBuffer(0));
	kkp2p_join_lan(kkp2p_engine, m_strPeerId.GetBuffer(0));

	kkp2p_channel_t channel;
	int iRet = 0;
	while (true) {
	    iRet = kkp2p_accept(kkp2p_engine, 1000, &channel);
		if (iRet < 0) {
			AfxMessageBox(A2T("kkp2p accept error"));
			return;
		}
		else if (iRet == 0) {
			continue;
		}
		else {
			break;
		}
	}

	// show connect type
	if (channel.transmit_mode == 1) {
		m_connectType.SetWindowText(_T("p2p"));
	}
	else if (channel.transmit_mode == 2) {
		m_connectType.SetWindowText(_T("relay"));
	}
	UpdateWindow();

	// protocol
	// fileSize(8 byte) + fileNameLen(2 byte) + fileName + fileBody
	uint64_t netFileSize;
	int ret = RecvData(channel.fd, (char*)&netFileSize, sizeof(uint64_t));
	if (ret < 0) {
		AfxMessageBox(A2T("recv file size error"));
		return;
	}
	uint64_t fileSize = Ntohll(netFileSize);

	uint16_t netNameLen;
	ret = RecvData(channel.fd, (char*)&netNameLen, sizeof(uint16_t));
	if (ret < 0) {
		AfxMessageBox(A2T("recv name len error"));
		return;
	}
	uint16_t nameLen = ntohs(netNameLen);

	char fileName[1024];
	memset(fileName, 0, sizeof(fileName));
	ret = RecvData(channel.fd, fileName, nameLen);
	if (ret < 0) {
		AfxMessageBox(A2T("recv name error"));
		return;
	}

	// create file
	char szPath[1024];
	sprintf(szPath,"%s\\%s.%lu", szDirName, fileName,time(NULL));
	FILE * pWrite = fopen(szPath, "wb");
	if (!pWrite)
	{
		AfxMessageBox(A2T("open file error"));
		return;
	}
	m_folderName.SetWindowTextA(szPath);
	UpdateWindow();

	// loop recv file body
	char buff[1024];
	uint64_t totalRecv = 0;
	int readLen = 0;
	uint64_t startM = TimeMilliSecond();
	uint64_t endM = startM;
	uint32_t cnt = 0;
	while (totalRecv < fileSize) {
		readLen = kkp2p_read(channel.fd, buff, sizeof(buff), 1000);
		if (readLen < 0) {
			AfxMessageBox(A2T("read file error"));
			return;
		}
		fwrite(buff, 1, readLen, pWrite);
		totalRecv += readLen;
		if (++cnt % 100 == 0) {
			endM = TimeMilliSecond();
			if (endM > startM) {
				int speed = totalRecv * 1000 / (endM - startM);
				char info[1024];
				sprintf(info, "file len:%lld,recv len:%lld,speed:%d(byte/s)", fileSize, totalRecv, speed);
				m_statInfo.SetWindowTextA(info);
				UpdateWindow();
			}
		}
	}
	endM = TimeMilliSecond();
	if (endM > startM) {
		int speed = totalRecv * 1000 / (endM - startM);
		char info[1024];
		sprintf(info, "file len:%lld,recv len:%lld,speed:%d(byte/s)", fileSize, totalRecv, speed);
		m_statInfo.SetWindowTextA(info);
		UpdateWindow();
	}

	fflush(pWrite);
	fclose(pWrite);

	// write ack
	char ch = '1';
	SendData(channel.fd, &ch, 1);
	kkp2p_close_fd(channel.fd);
	kkp2p_close_channel(kkp2p_engine, channel.channel_id);

	char info[1024];
	sprintf(info, "recv finished,file len:%llu,recv len:%llu", fileSize, totalRecv);
	AfxMessageBox(A2T(info));

	Sleep(2); // wait ack sended finish
	kkp2p_engine_destroy(kkp2p_engine);
}
