/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：大数操作 + - * /
版本号：1.0.0
开发时期：1999
作者：ln
修改记录：
******************************************************/

#include <ewindows.h>
#include "largenum.h"

UINT32
LargeNumSignedFormat(
    PLARGENUM   pNum
    );

// **************************************************
// 声明：PLARGENUM LargeNumSet( 
//					PLARGENUM pNum,
//					int n
//					)
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
//	IN n - 需要设置的数
// 返回值：
//	返回初始化好的 PLARGENUM 指针
// 功能描述：
//	初始化大数对象
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumSet( 
					PLARGENUM pNum,
					int n
					)
{
    int i;

    if(n < 0){
        pNum->u.s32.u[0] = -n;
        pNum->fNegative = 1;
    } else{
        pNum->u.s32.u[0] = n;
        pNum->fNegative=0;
    }
    for(i=1; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] = 0;
    }
    return pNum;
}

// **************************************************
// 声明：BOOL IsLargeNumNotZero( PLARGENUM   pNum )
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
// 返回值：
//	假如是非0值,返回TRUE；否则，返回FALSE
// 功能描述：
//	判断大数是否为非0值
// 引用: 
//	
// ************************************************

BOOL IsLargeNumNotZero(
					PLARGENUM   pNum
					)
{
    int i;

    for(i=0; i<SIZE_OF_LARGENUM; i++){
        if(pNum->u.s32.u[i]){
            return TRUE;
        }
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL IsLargeNumNegative( PLARGENUM pNum )
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
// 返回值：
//	假如是负值,返回TRUE；否则，返回FALSE
// 功能描述：
//	判断大数是否为负值
// 引用: 
//	
// ************************************************

BOOL IsLargeNumNegative( PLARGENUM pNum )
{
    return (pNum->fNegative ? TRUE : FALSE);
}

// **************************************************
// 声明：BOOL IsLargeNumMagGreaterThan(
//							PLARGENUM   pNum1,
//							PLARGENUM   pNum2
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 返回值：
//	假如pNum1 > pNum2 ,返回TRUE；否则，返回FALSE
// 功能描述：
//	判断大数 pNum1 是否大于 pNum2（无符号比较）
// 引用: 
//	
// ************************************************

BOOL IsLargeNumMagGreaterThan(
							PLARGENUM   pNum1,
							PLARGENUM   pNum2
    )
{
    int i;

    for(i=SIZE_OF_LARGENUM-1; i>=0; i--){
        if(pNum1->u.s32.u[i] > pNum2->u.s32.u[i]){
            return TRUE;
        } else if(pNum1->u.s32.u[i] < pNum2->u.s32.u[i]){
            return FALSE;
        }
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL IsLargeNumMagLessThan(
//						PLARGENUM   pNum1,
//						PLARGENUM   pNum2
//						)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 返回值：
//	假如pNum1 < pNum2 ,返回TRUE；否则，返回FALSE
// 功能描述：
//	判断大数 pNum1 是否小于 pNum2（无符号比较）
// 引用: 
//	
// ************************************************

BOOL IsLargeNumMagLessThan(
						PLARGENUM   pNum1,
						PLARGENUM   pNum2
						)
{
    int i;

    for(i=SIZE_OF_LARGENUM-1; i>=0; i--){
        if(pNum1->u.s32.u[i] < pNum2->u.s32.u[i]){
            return TRUE;
        } else if(pNum1->u.s32.u[i] > pNum2->u.s32.u[i]){
            return FALSE;
        }
    }
    return FALSE;
}

// **************************************************
// 声明：PLARGENUM LargeNumMagInc( PLARGENUM pNum )
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
// 返回值
//	返回 PLARGENUM 对象指针
// 功能描述：
//	大数加一（无符号增加）
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMagInc( PLARGENUM pNum )
{
    UINT32  c;
    int     i;

    c = 1;
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] += c;
        if(pNum->u.s32.u[i]){
            c = 0;
        }
    }
    return pNum;
}

// **************************************************
// 声明：PLARGENUM LargeNumMagAdd(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 无符号 pResult = pNum1 + pNum2 的功能
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMagAdd(
					PLARGENUM   pNum1,
					PLARGENUM   pNum2,
					PLARGENUM   pResult
					)
{
    UINT32      c;
    UINT32      i;
    UINT32      a;
    UINT32      b;

    c = 0;	//进位
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        a = pNum1->u.s32.u[i];
        b = pNum2->u.s32.u[i];
        pResult->u.s32.u[i] = a + b + c;
		//低位有进位吗 ？
        if(c){
			//低位有进位
			// [c=1][u[i]=5] = 9[a] + 6[b] + [c]
            if(pResult->u.s32.u[i] <= a){
                c = 1;	//溢出，需要向高位进位
            } else {
                c = 0;
            }
        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;	//溢出，需要向高位进位
            } else {
                c = 0;
            }
        }
    }
    return pResult;
}

// **************************************************
// 声明：PLARGENUM LargeNumMagSub(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 无符号 pResult = pNum1 - pNum2 的功能
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMagSub(
					PLARGENUM   pNum1,
					PLARGENUM   pNum2,
					PLARGENUM   pResult
					)
{
    UINT32      c;
    UINT32      i;
    UINT32      a;
    UINT32      b;

    c = 1;
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        a = pNum1->u.s32.u[i];
        b = ~(pNum2->u.s32.u[i]);   // = -b
        pResult->u.s32.u[i] = a + b + c;
		//低位有进位吗 ？
        if(c){
			//低位有进位
			// [c=1][u[i]=5] = 9[a] + 6[b] + [c]
            if(pResult->u.s32.u[i] <= a){
                c = 1;	//溢出，需要向高位进位
            } else {
                c = 0;
            }

        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;	//溢出，需要向高位进位
            } else {
                c = 0;
            }

        }
    }
    return pResult;
}

// **************************************************
// 声明：PLARGENUM LargeNumAdd(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 pResult = pNum1 + pNum2 的功能
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumAdd(
					PLARGENUM   pNum1,
					PLARGENUM   pNum2,
					PLARGENUM   pResult
					)
{
    BOOL    fNegative1;
    BOOL    fNegative2;
	//判断正负
    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 != fNegative2){
		//不同符号
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
			//绝对值 abs(pNum1) > abs(pNum2)
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
			//绝对值 abs(pNum1) < abs(pNum2)
            LargeNumMagSub(pNum2, pNum1, pResult);
			//符号反向
            fNegative1 = !fNegative1;
        }
    } else {
		//同符号
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
	//确定结果的符号
	//判断结果是否为0
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;			//为0
    } else {
        pResult->fNegative = fNegative1;	//非0
    }
    return pResult;
}

// **************************************************
// 声明：PLARGENUM LargeNumSub(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 pResult = pNum1 - pNum2 的功能
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumSub(
					PLARGENUM   pNum1,
					PLARGENUM   pNum2,
					PLARGENUM   pResult
					)
{
    BOOL    fNegative1;
    BOOL    fNegative2;
	
	//判断正负
    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 == fNegative2){
		//相同符号
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
			//绝对值 abs(pNum1) > abs(pNum2)
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
			//绝对值 abs(pNum1) < abs(pNum2)
            LargeNumMagSub(pNum2, pNum1, pResult);
            fNegative1 = !fNegative1;//符号反向
        }
    } else {
		//不同符号，like a - (-b) or -a - b
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
	//确定结果的符号
    if(!IsLargeNumNotZero(pResult)){
		//结果为0
        pResult->fNegative = FALSE;
    } else {
        pResult->fNegative = fNegative1;
    }
    return pResult;
}

#if SIZE_OF_LARGENUM < 2
#error  SIZE_OF_LARGENUM must be at least 2
#endif

// **************************************************
// 声明：PLARGENUM LargeNumMulUint32(
//						UINT32      a,
//						UINT32      b,
//						PLARGENUM   pResult
//						)
// 参数：
// 	IN a - 值1
// 	IN b - 值2
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 pResult = a * b; 无符号乘
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMulUint32(
						UINT32      a,
						UINT32      b,
						PLARGENUM   pResult
						)
{
    UINT32  a1, a0;
    UINT32  b1, b0;
    UINT32  r0;
    UINT32  r1;
    UINT32  r2;
    UINT32  c;
    int     i;

    a1 = a >> 16;		//a的高16位
    a0 = a & 0xffff;	//a的低16位
    b1 = b >> 16;		//b的高16位
    b0 = b & 0xffff;	//b的低16位

	// (a1*65536 + a0) * (b1*65536 + b0)
	//	= a0 * b0 + a1 * 65536 * b0 + a0 * b1 * 65536 + a1 * b1 * 65536 * 65536
	//  = a0 * b0 + ( ( a1 * b0 + a0 * b1 )<<16 ) + a1 * b1 * 65536 * 65536
	//  = r0      +  (r1 << 16)                   + r2 * 65536 * 65536
    r0 = a0 * b0;
    r1 = a1 * b0 + a0 * b1;
    r2 = a1 * b1;
	//底 32位
    pResult->u.s32.u[0] = (r1 << 16) + r0;
	//是否上溢 ？
    if(pResult->u.s32.u[0] < r0){
        c = 1;	//上溢
    } else {
        c = 0;
    }
	//高 32位
    pResult->u.s32.u[1] = r2 + (r1 >> 16) + c;
    for(i=2; i<SIZE_OF_LARGENUM; i++){
        pResult->u.s32.u[i] = 0;
    }
    pResult->fNegative = 0;

    return pResult;
}

// **************************************************
// 声明：PLARGENUM LargeNumMulInt32(
//						int       a,
//						int       b,
//						PLARGENUM   pResult
// 参数：
// 	IN a - 值1
// 	IN b - 值2
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 pResult = a * b; 带符号乘
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMulInt32(
						int       a,
						int       b,
						PLARGENUM   pResult
    )
{
    BOOL        fNegativeA;
    BOOL        fNegativeB;
	//检查符号
    if(a < 0){
        fNegativeA = TRUE;
        a = -a;	//转化为无符号
    } else {
        fNegativeA = FALSE;
    }
	//检查符号
    if(b < 0){
        fNegativeB = TRUE;
        b = -b;	//转化为无符号
    } else {
        fNegativeB = FALSE;
    }
	//无符号乘
    LargeNumMulUint32(a, b, pResult);
	//结果是0 ?
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;		//是 0
    } else {
        if(fNegativeA != fNegativeB){
            pResult->fNegative = TRUE;
        }
    }
    return pResult;
}

// **************************************************
// 声明：PLARGENUM LargeNumMult( 
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// 参数：
// 	IN pNum1 - LARGENUM 结构指针，大数对象
// 	IN pNum2 - LARGENUM 结构指针，大数对象
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 PLARGENUM 结果对象指针
// 功能描述：
//	实现 pResult = pNum1 * pNum2; 带符号乘
// 引用: 
//	
// ************************************************

PLARGENUM LargeNumMult( 
					PLARGENUM   pNum1,
					PLARGENUM   pNum2,
					PLARGENUM   pResult
					)
{
    LARGENUM    lNumTemp;
    LARGENUM    lNumSum;
    LARGENUM    lNumCarry;
    int         i;
    int         j;
	//初始化 为 0
    LargeNumSet(&lNumCarry, 0);
    for(i=0; i<SIZE_OF_LARGENUM; i++){
		//初始化 为 0
        LargeNumSet(&lNumSum, 0);
        for(j=0; j<=i; j++){
			//lNumTemp = pNum1->u.s32.u[j] * pNum2->u.s32.u[i-j]
            LargeNumMulUint32(pNum1->u.s32.u[j], pNum2->u.s32.u[i-j], &lNumTemp);
			//lNumSum += lNumTemp
            LargeNumMagAdd(&lNumTemp, &lNumSum, &lNumSum);
        }
		//lNumSum += lNumCarry
        LargeNumMagAdd(&lNumCarry, &lNumSum, &lNumSum);
        for(j=0; j<SIZE_OF_LARGENUM-1; j++){
            lNumCarry.u.s32.u[j] = lNumSum.u.s32.u[j+1];
        }
        pResult->u.s32.u[i] = lNumSum.u.s32.u[0];
    }

    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;
    } else {
        pResult->fNegative = (pNum1->fNegative != pNum2->fNegative);
    }
    return pResult;
}

// **************************************************
// 声明：UINT32 LargeNumSignedFormat(
//						PLARGENUM   pNum
//						)
// 参数：
// 	IN/OUT pNum - LARGENUM 结构指针，大数对象
// 返回值：
//	如果是负数，返回0xffffffff；否则，返回0
// 功能描述：
//	转化负大数为标准的负数格式（高bit位扩展为1）
// 引用: 
//	
// ************************************************

UINT32 LargeNumSignedFormat(
						PLARGENUM   pNum
						)
{
    int     i;
    UINT32  c;

    if(IsLargeNumNegative(pNum)){
        c = 1;
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pNum->u.s32.u[i] = ~(pNum->u.s32.u[i]) + c;
            if(pNum->u.s32.u[i]){
                c = 0;
            }
        }
        return 0xffffffff;
    } else {
        return 0;
    }
}

// **************************************************
// 声明：VOID LargeNumRAShift(
//					PLARGENUM   pNum,
//					int       count
//					)
// 参数：
// 	IN/OUT pNum - LARGENUM 结构指针，大数对象
//	IN count - 右移位数
// 返回值：
//	无
// 功能描述：
//	向右移位
// 引用: 
//	
// ************************************************

VOID LargeNumRAShift(
					PLARGENUM   pNum,
					int       count
					)
{
    int   shift32;
    int   countLeft;
    UINT32  filler;
    int     i;
    int     j;
	
	//格式化为标准的数
    filler = LargeNumSignedFormat(pNum);

    shift32 = count / 32;

    if(shift32 > (SIZE_OF_LARGENUM - 1)){
		//全部移出
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pNum->u.s32.u[i] = filler;
        }
        return;
    }
	//移位
    count %= 32;
    countLeft = 32 - count;
    for(i=0, j=shift32;;){
        pNum->u.s32.u[i] = (pNum->u.s32.u[j] >> count);
        if(j<(SIZE_OF_LARGENUM-1)){
            j++;
            pNum->u.s32.u[i] |= pNum->u.s32.u[j] << countLeft;
            i++;
        } else {
            pNum->u.s32.u[i] |= filler << countLeft;
            i++;
            break;
        }
    }
	//填充剩下的
    for(; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] = filler;
    }
}

// **************************************************
// 声明：UINT LargeNumDivint32(
//				PLARGENUM   pNum,
//				int       divisor,
//				PLARGENUM   pResult
//				)
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
//	IN divisor - 除数
// 	OUT pResult - LARGENUM 结构指针，大数对象
// 返回值：
//	返回余数
// 功能描述：
//	pResult = pNum / divisor
// 引用: 
//	
// ************************************************

UINT LargeNumDivint32(
				PLARGENUM   pNum,
				int       divisor,
				PLARGENUM   pResult
				)
{
    UINT32  s[2*SIZE_OF_LARGENUM];
    UINT32  r;
    UINT32  q;
    UINT32  d;
    BOOL    sd;
    int     i;

    for(i=0; i<2*SIZE_OF_LARGENUM; i++){
        s[i] = pNum->u.s16.s[i];
    }

    if(divisor < 0){
		//负数
        divisor = -divisor;
        sd = TRUE;
    } else if(divisor == 0){
		//被 0 除
        //
        // This is a divide-by-zero error
        //
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pResult->u.s32.u[i] = 0xffffffff;
        }
        return 0xffffffff;
    } else {
		//非 0 正数
        sd = FALSE;
    }
	//逐位除
    r = 0;
    for(i=(2*SIZE_OF_LARGENUM-1); i>=0; i--){
        d = (r << 16) + s[i];
        q = d / divisor;
        r = d - q * divisor;
        s[i] = q;
    }
	//保存结果
    for(i=0; i<2*SIZE_OF_LARGENUM; i++){
        pResult->u.s16.s[i] = s[i];
    }

    if(pNum->fNegative){
		//被除数是正数  +1
        LargeNumMagInc(pResult);
		//余数
        r = divisor - r;
		//得到结果的符号
        if(sd == 0 && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }

    } else {
		//被除数是正数
        if(sd && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }
    }

    return r;
}

// **************************************************
// 声明：int LargeNumBits(
//				PLARGENUM   pNum
//				)
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
// 返回值：
//	返回大数的最高有效bit位
// 功能描述：
//	得到大数的最高有效bit位
// 引用: 
//	
// ************************************************

int LargeNumBits(
				PLARGENUM   pNum
				)
{
    static const UINT32 LargeNumMask[32] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080,
        0x00000100,
        0x00000200,
        0x00000400,
        0x00000800,
        0x00001000,
        0x00002000,
        0x00004000,
        0x00008000,
        0x00010000,
        0x00020000,
        0x00040000,
        0x00080000,
        0x00100000,
        0x00200000,
        0x00400000,
        0x00800000,
        0x01000000,
        0x02000000,
        0x04000000,
        0x08000000,
        0x10000000,
        0x20000000,
        0x40000000,
        0x80000000,
        };

    int     i;
    int     j;
    UINT32  u;

    for(i=(SIZE_OF_LARGENUM-1); i>=0; i--){
        u = pNum->u.s32.u[i];
        if(u){
            for(j=31; j>=0; j--){
                if(u & (LargeNumMask[j])){
                    return i * 32 + j + 1;
                }
            }
        }
    }
    return 0;
}

// **************************************************
// 声明：char * LargeNumToAscii(
//								PLARGENUM   pNum
//								)
// 参数：
// 	IN pNum - LARGENUM 结构指针，大数对象
// 返回值：
//	返回 ascii 指针
// 功能描述：
//	将大数转化为字符串
// 引用: 
//	
// ************************************************

char * LargeNumToAscii(
						PLARGENUM   pNum
					)
{
    static  char    buf[SIZE_OF_LARGENUM * 10 + 2];
    LARGENUM        lNum;
    char            *p;
    char            *q;
    UINT32          r;
    int             s;

    p = buf + sizeof(buf) - 1;
    *p= 0;

    lNum = *pNum;

    s = pNum->fNegative;
    lNum.fNegative = 0;

    while(IsLargeNumNotZero(&lNum)){
		// r = num % 10;  lNum /= 10;
        r = LargeNumDivint32(&lNum, 10, &lNum);
        p--;
        *p = r + '0';
    }

    q = buf;
	//因为低位数放在 字符指针的开始地址，所以需要反转
    if(s){
        *q++='-';
    }
    while(*p){
        *q++ = *p++;
    }

    if((q == buf) || (s && q == &(buf[1]))){
        *q++ = '0';
    }
    *q = 0;
    return buf;
}
