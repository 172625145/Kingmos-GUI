/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�����ļ�·������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-03-29
���ߣ�JAMI CHEN �½���
�޸ļ�¼��
**************************************************/
//#include <eversion.h>
#include <efilepth.h>

// **************************************************
// ������void _splitpath( const char *path, char *drive, char *dir, char *fname, char *ext)
// ������
// 	IN path -- ��ǰҪ�����·��
// 	OUT drive -- �������������Ļ���
// 	OUT dir -- ����·���Ļ���
// 	OUT fname -- �����ļ����Ļ���
// 	OUT ext -- ������չ���Ļ���
// 
// ����ֵ����
// ��������������ǰ·������ɸ���ָ��������
// ����: 
// **************************************************
void _splitpath( const char *path, char *drive, char *dir, char *fname, char *ext)
{
	const char *ptr = path;
	int length;

//		ptr=path;
		// Search '.'
		while(*ptr)
		{
			if (*ptr=='.')
				break;
			ptr++;
		}
		// Get ext
		if (ext)
		{
			strcpy(ext,ptr);
		}
		// Get filename
		length=0;
		while(ptr>path)
		{
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\' || *ptr =='/')
			{
				ptr++;  // not include '\' 
				break;
			}
			length++;
		}
		if (fname)
		{
			strncpy(fname,ptr,length);
			fname[length]=0;
		}

		//get dir
		length=0;
		while(ptr>path)
		{
			ptr--;  // include '\' 
			if (*ptr==':')
			{
				ptr++;  // not include ':' 
				break;
			}
			length++;
		}
		if (dir)
		{
			strncpy(dir,ptr,length);
			dir[length]=0;
		}

		//get drive
		length=ptr-path;
		if (drive)
		{
			strncpy(drive,path,length);
			drive[length]=0;
		}
}
/*
#ifdef KINGMOS_UNICODE

void _wsplitpath( const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *fname, wchar_t *ext )
{
	wchar_t *ptr;
	int length;

		ptr=path;
		// Search '.'
		while(*ptr)
		{
			if (*ptr=='.')
				break;
			ptr++;
		}
		// Get ext
		if (ext)
		{
			//wcscpy(ext,ptr);
		}
		// Get filename
		length=0;
		while(ptr>path)
		{
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\')
			{
				ptr++;  // not include '\' 
				break;
			}
			length++;
		}
		if (fname)
		{
			wcsncpy(fname,ptr,length);
			fname[length]=0;
		}

		//get dir
		length=0;
		while(ptr>path)
		{
			ptr--;  // include '\' 
			if (*ptr==':')
			{
				ptr++;  // not include ':' 
				break;
			}
			length++;
		}
		if (dir)
		{
			wcsncpy(dir,ptr,length);
			dir[length]=0;
		}

		//get drive
		length=ptr-path;
		if (drive)
		{
			wcsncpy(drive,path,length);
			drive[length]=0;
		}
}
#endif
*/
// **************************************************
// ������void _makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext )
// ������
// 	OUT path -- ����ϲ����·���Ļ���
// 	IN drive -- ��ǰ����������
// 	IN dir -- ��ǰ��·��
// 	IN fname -- ��ǰ���ļ���
// 	IN ext -- ��ǰ��չ��
// 
// ����ֵ����
// ��������������ǰ����ָ�������ݺϲ���·�������·����
// ����: 
// **************************************************
void _makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext )
{
        register const char *p;


        if (drive && *drive) {
                *path++ = *drive;
                *path++ = ':';
        }

        /* copy dir */

        if ((p = dir) && *p) {
            do 
			{
                    *path++ = *p++;
            }
            while (*p);

            if (*(p-1) != '/' && *(p-1) != '\\')
			{
                    *path++ = '\\';
            }
        }

        /* copy fname */

        if (p = fname) 
		{
                while (*p) 
				{
                        *path++ = *p++;
                }
        }


        if (p = ext) 
		{
            if (*p && *p != '.') {
                    *path++ = '.';
            }
            while (*path++ = *p++)
                    ;
        }
        else {
                /* better add the 0-terminator */
                *path = '\0';
        }
}

/*
#ifdef KINGMOS_UNICODE

void _wmakepath( wchar_t *path, const wchar_t *drive, const wchar_t *dir, const wchar_t *fname, const wchar_t *ext )
{
        register const wchar_t *p;


        if (drive && *drive) {
                *path++ = *drive;
                *path++ = ':';
        }

        // copy dir

        if ((p = dir) && *p) {
            do 
			{
                    *path++ = *p++;
            }
            while (*p);

            if (*(p-1) != '/' && *(p-1) != '\\')
			{
                    *path++ = _T('\\');
            }
        }

        // copy fname

        if (p = fname) 
		{
                while (*p) 
				{
                        *path++ = *p++;
                }
        }


        if (p = ext) 
		{
            if (*p && *p != '.') {
                    *path++ = '.';
            }
            while (*path++ = *p++)
                    ;
        }
        else {
                // better add the 0-terminator
                *path = '\0';
        }
}

#endif
*/
