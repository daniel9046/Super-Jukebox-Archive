// ListEntry.h: interface for the CListEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENTRY_H__726DDCA2_8411_11D4_8925_009027C5DD32__INCLUDED_)
#define AFX_LISTENTRY_H__726DDCA2_8411_11D4_8925_009027C5DD32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListEntry  
{
public:
	void GetPublisher(LPTSTR lpszPublisher,INT nTextMax);
	void SetPublisher(LPCTSTR lpszPublisher);
	FILETIME GetModified();
	void SetModified(FILETIME ftModified);
	void GetType(LPTSTR lpszType,INT nTextMax);
	void SetType(LPCTSTR lpszType);
	LONGLONG GetSize();
	void SetSize(LONGLONG llSize);
	BOOL GetPath(LPTSTR lpszPath,INT nTextMax);
	void ReleaseValidFileName();
	INT GetValidFileName(LPTSTR lpszFileName,INT nTextMax);
	void GetRedirectedName(LPTSTR lpszName,INT nTextMax);
	INT GetListIndex();
	void SetListIndex(INT nIndex);
	UINT GetListID();
	void SetListID(UINT uListID);
	void IsChanged(BOOL bChanged);
	BOOL IsChanged();
	BOOL SetProperty(LPCTSTR lpszPropertyName,LPCTSTR lpszNewValue);
	BOOL GetProperty(LPCTSTR lpszPropertyName,LPTSTR lpszBuffer,INT nTextMax);
	void IsAdded(BOOL bAdded);
	BOOL IsAdded();
	void IsRedirected(BOOL bRedirected);
	BOOL IsRedirected();
	void RedirectFile(LPCTSTR lpszFileName);
	BOOL LoadID666Tag();
	BOOL SaveID666Tag();
	void HasID666Tag(BOOL bHasID666Tag);
	BOOL HasID666Tag();
	void IsArchived(BOOL bArchived);
	BOOL IsArchived();
	BOOL SetID666Tag(ID666TAG& Tag);
	void GetID666Tag(ID666TAG& Tag);
	void Clear();
	void IsDisplayed(BOOL bDisplayed);
	BOOL IsDisplayed();
	INT GetFadeLength();
	void SetFadeLength(INT nFadeLength);
	INT GetLength();
	void SetLength(INT nLength);
	void GetComment(LPTSTR lpszComment,INT nTextMax);
	void SetComment(LPCTSTR lpszComent);
	void GetDate(LPTSTR lpszDate,INT nTextMax);
	void SetDate(LPCTSTR lpszDate);
	void GetDumper(LPTSTR lpszDumper,INT nTextMax);
	void SetDumper(LPCTSTR lpszDumper);
	void GetArtist(LPTSTR lpszArist,INT nTextMax);
	void SetArtist(LPCTSTR lpszArtist);
	void GetGame(LPTSTR lpszGame,INT nTextMax);
	void SetGame(LPCTSTR lpszGame);
	void GetTitle(LPTSTR lpszTitle,INT nTextMax);
	void SetTitle(LPCTSTR lpszTitle);
	INT GetType();
	void SetType(INT nType);
	INT GetIcon();
	void SetIcon(INT iIcon);
	void GetDirectory(LPTSTR lpszName,INT nTextMax);
	void SetDirectory(LPCTSTR lpszDirectory);
	void GetFullName(LPTSTR lpszName,INT nTextMax);
	void SetFullName(LPCTSTR lpszFullName);
	void GetName(LPTSTR lpszName,INT nTextMax);
	void SetName(LPCTSTR lpszName);
	bool operator<(const CListEntry & obj);
	CListEntry();
	virtual ~CListEntry();

private:
	FILETIME m_ftModified;
	TCHAR m_szType[500];
	LONGLONG m_llSize;
	TCHAR m_szValidFileName[MAX_PATH];
	INT m_nListIndex;
	UINT m_uListID;
	BOOL m_bChanged;
	BOOL m_bAdded;
	BOOL m_bRedirected;
	BOOL m_bHasID666Tag;
	BOOL m_bArchived;
	BOOL m_bDisplayed;
	ID666TAG m_ID666Tag;
	TCHAR m_szName[MAX_PATH];
	TCHAR m_szFullName[MAX_PATH];
	TCHAR m_szDirectory[MAX_PATH];
	TCHAR m_szRedirectedFileName[MAX_PATH];
	INT m_nType,m_iIcon;
};

#endif // !defined(AFX_LISTENTRY_H__726DDCA2_8411_11D4_8925_009027C5DD32__INCLUDED_)
