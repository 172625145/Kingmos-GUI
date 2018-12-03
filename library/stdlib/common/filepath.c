/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：文件路径处理
版本号：1.0.0.456
开发时期：2002-03-29
作者：JAMI CHEN 陈建明
修改记录：
**************************************************/
//#include <eversion.h>
#include <efilepth.h>

// **************************************************
// 声明：void _splitpath( const char *path, char *drive, char *dir, char *fname, char *ext)
// 参数：
// 	IN path -- 当前要分离的路径
// 	OUT drive -- 保存驱动器名的缓存
// 	OUT dir -- 保存路径的缓存
// 	OUT fname -- 保存文件名的缓存
// 	OUT ext -- 保存扩展名的缓存
// 
// 返回值：无
// 功能描述：将当前路径分离成各种指定的内容
// 引用: 
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
// 声明：void _makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext )
// 参数：
// 	OUT path -- 保存合并后的路径的缓存
// 	IN drive -- 当前的驱动器名
// 	IN dir -- 当前的路径
// 	IN fname -- 当前的文件名
// 	IN ext -- 当前扩展名
// 
// 返回值：无
// 功能描述：将当前各种指定的内容合并成路径分离成路径。
// 引用: 
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
