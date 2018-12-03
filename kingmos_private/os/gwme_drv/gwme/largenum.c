/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������������ + - * /
�汾�ţ�1.0.0
����ʱ�ڣ�1999
���ߣ�ln
�޸ļ�¼��
******************************************************/

#include <ewindows.h>
#include "largenum.h"

UINT32
LargeNumSignedFormat(
    PLARGENUM   pNum
    );

// **************************************************
// ������PLARGENUM LargeNumSet( 
//					PLARGENUM pNum,
//					int n
//					)
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
//	IN n - ��Ҫ���õ���
// ����ֵ��
//	���س�ʼ���õ� PLARGENUM ָ��
// ����������
//	��ʼ����������
// ����: 
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
// ������BOOL IsLargeNumNotZero( PLARGENUM   pNum )
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	�����Ƿ�0ֵ,����TRUE�����򣬷���FALSE
// ����������
//	�жϴ����Ƿ�Ϊ��0ֵ
// ����: 
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
// ������BOOL IsLargeNumNegative( PLARGENUM pNum )
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	�����Ǹ�ֵ,����TRUE�����򣬷���FALSE
// ����������
//	�жϴ����Ƿ�Ϊ��ֵ
// ����: 
//	
// ************************************************

BOOL IsLargeNumNegative( PLARGENUM pNum )
{
    return (pNum->fNegative ? TRUE : FALSE);
}

// **************************************************
// ������BOOL IsLargeNumMagGreaterThan(
//							PLARGENUM   pNum1,
//							PLARGENUM   pNum2
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	����pNum1 > pNum2 ,����TRUE�����򣬷���FALSE
// ����������
//	�жϴ��� pNum1 �Ƿ���� pNum2���޷��űȽϣ�
// ����: 
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
// ������BOOL IsLargeNumMagLessThan(
//						PLARGENUM   pNum1,
//						PLARGENUM   pNum2
//						)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	����pNum1 < pNum2 ,����TRUE�����򣬷���FALSE
// ����������
//	�жϴ��� pNum1 �Ƿ�С�� pNum2���޷��űȽϣ�
// ����: 
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
// ������PLARGENUM LargeNumMagInc( PLARGENUM pNum )
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
// ����ֵ
//	���� PLARGENUM ����ָ��
// ����������
//	������һ���޷������ӣ�
// ����: 
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
// ������PLARGENUM LargeNumMagAdd(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� �޷��� pResult = pNum1 + pNum2 �Ĺ���
// ����: 
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

    c = 0;	//��λ
    for(i=0; i<SIZE_OF_LARGENUM; i++){
        a = pNum1->u.s32.u[i];
        b = pNum2->u.s32.u[i];
        pResult->u.s32.u[i] = a + b + c;
		//��λ�н�λ�� ��
        if(c){
			//��λ�н�λ
			// [c=1][u[i]=5] = 9[a] + 6[b] + [c]
            if(pResult->u.s32.u[i] <= a){
                c = 1;	//�������Ҫ���λ��λ
            } else {
                c = 0;
            }
        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;	//�������Ҫ���λ��λ
            } else {
                c = 0;
            }
        }
    }
    return pResult;
}

// **************************************************
// ������PLARGENUM LargeNumMagSub(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� �޷��� pResult = pNum1 - pNum2 �Ĺ���
// ����: 
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
		//��λ�н�λ�� ��
        if(c){
			//��λ�н�λ
			// [c=1][u[i]=5] = 9[a] + 6[b] + [c]
            if(pResult->u.s32.u[i] <= a){
                c = 1;	//�������Ҫ���λ��λ
            } else {
                c = 0;
            }

        } else {
            if(pResult->u.s32.u[i] < a){
                c = 1;	//�������Ҫ���λ��λ
            } else {
                c = 0;
            }

        }
    }
    return pResult;
}

// **************************************************
// ������PLARGENUM LargeNumAdd(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� pResult = pNum1 + pNum2 �Ĺ���
// ����: 
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
	//�ж�����
    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 != fNegative2){
		//��ͬ����
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
			//����ֵ abs(pNum1) > abs(pNum2)
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
			//����ֵ abs(pNum1) < abs(pNum2)
            LargeNumMagSub(pNum2, pNum1, pResult);
			//���ŷ���
            fNegative1 = !fNegative1;
        }
    } else {
		//ͬ����
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
	//ȷ������ķ���
	//�жϽ���Ƿ�Ϊ0
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;			//Ϊ0
    } else {
        pResult->fNegative = fNegative1;	//��0
    }
    return pResult;
}

// **************************************************
// ������PLARGENUM LargeNumSub(
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� pResult = pNum1 - pNum2 �Ĺ���
// ����: 
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
	
	//�ж�����
    fNegative1 = IsLargeNumNegative(pNum1);
    fNegative2 = IsLargeNumNegative(pNum2);

    if(fNegative1 == fNegative2){
		//��ͬ����
        if(IsLargeNumMagGreaterThan(pNum1, pNum2)){
			//����ֵ abs(pNum1) > abs(pNum2)
            LargeNumMagSub(pNum1, pNum2, pResult);
        } else {
			//����ֵ abs(pNum1) < abs(pNum2)
            LargeNumMagSub(pNum2, pNum1, pResult);
            fNegative1 = !fNegative1;//���ŷ���
        }
    } else {
		//��ͬ���ţ�like a - (-b) or -a - b
        LargeNumMagAdd(pNum1, pNum2, pResult);
    }
	//ȷ������ķ���
    if(!IsLargeNumNotZero(pResult)){
		//���Ϊ0
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
// ������PLARGENUM LargeNumMulUint32(
//						UINT32      a,
//						UINT32      b,
//						PLARGENUM   pResult
//						)
// ������
// 	IN a - ֵ1
// 	IN b - ֵ2
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� pResult = a * b; �޷��ų�
// ����: 
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

    a1 = a >> 16;		//a�ĸ�16λ
    a0 = a & 0xffff;	//a�ĵ�16λ
    b1 = b >> 16;		//b�ĸ�16λ
    b0 = b & 0xffff;	//b�ĵ�16λ

	// (a1*65536 + a0) * (b1*65536 + b0)
	//	= a0 * b0 + a1 * 65536 * b0 + a0 * b1 * 65536 + a1 * b1 * 65536 * 65536
	//  = a0 * b0 + ( ( a1 * b0 + a0 * b1 )<<16 ) + a1 * b1 * 65536 * 65536
	//  = r0      +  (r1 << 16)                   + r2 * 65536 * 65536
    r0 = a0 * b0;
    r1 = a1 * b0 + a0 * b1;
    r2 = a1 * b1;
	//�� 32λ
    pResult->u.s32.u[0] = (r1 << 16) + r0;
	//�Ƿ����� ��
    if(pResult->u.s32.u[0] < r0){
        c = 1;	//����
    } else {
        c = 0;
    }
	//�� 32λ
    pResult->u.s32.u[1] = r2 + (r1 >> 16) + c;
    for(i=2; i<SIZE_OF_LARGENUM; i++){
        pResult->u.s32.u[i] = 0;
    }
    pResult->fNegative = 0;

    return pResult;
}

// **************************************************
// ������PLARGENUM LargeNumMulInt32(
//						int       a,
//						int       b,
//						PLARGENUM   pResult
// ������
// 	IN a - ֵ1
// 	IN b - ֵ2
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� pResult = a * b; �����ų�
// ����: 
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
	//������
    if(a < 0){
        fNegativeA = TRUE;
        a = -a;	//ת��Ϊ�޷���
    } else {
        fNegativeA = FALSE;
    }
	//������
    if(b < 0){
        fNegativeB = TRUE;
        b = -b;	//ת��Ϊ�޷���
    } else {
        fNegativeB = FALSE;
    }
	//�޷��ų�
    LargeNumMulUint32(a, b, pResult);
	//�����0 ?
    if(!IsLargeNumNotZero(pResult)){
        pResult->fNegative = FALSE;		//�� 0
    } else {
        if(fNegativeA != fNegativeB){
            pResult->fNegative = TRUE;
        }
    }
    return pResult;
}

// **************************************************
// ������PLARGENUM LargeNumMult( 
//					PLARGENUM   pNum1,
//					PLARGENUM   pNum2,
//					PLARGENUM   pResult
//					)
// ������
// 	IN pNum1 - LARGENUM �ṹָ�룬��������
// 	IN pNum2 - LARGENUM �ṹָ�룬��������
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� PLARGENUM �������ָ��
// ����������
//	ʵ�� pResult = pNum1 * pNum2; �����ų�
// ����: 
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
	//��ʼ�� Ϊ 0
    LargeNumSet(&lNumCarry, 0);
    for(i=0; i<SIZE_OF_LARGENUM; i++){
		//��ʼ�� Ϊ 0
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
// ������UINT32 LargeNumSignedFormat(
//						PLARGENUM   pNum
//						)
// ������
// 	IN/OUT pNum - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	����Ǹ���������0xffffffff�����򣬷���0
// ����������
//	ת��������Ϊ��׼�ĸ�����ʽ����bitλ��չΪ1��
// ����: 
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
// ������VOID LargeNumRAShift(
//					PLARGENUM   pNum,
//					int       count
//					)
// ������
// 	IN/OUT pNum - LARGENUM �ṹָ�룬��������
//	IN count - ����λ��
// ����ֵ��
//	��
// ����������
//	������λ
// ����: 
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
	
	//��ʽ��Ϊ��׼����
    filler = LargeNumSignedFormat(pNum);

    shift32 = count / 32;

    if(shift32 > (SIZE_OF_LARGENUM - 1)){
		//ȫ���Ƴ�
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pNum->u.s32.u[i] = filler;
        }
        return;
    }
	//��λ
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
	//���ʣ�µ�
    for(; i<SIZE_OF_LARGENUM; i++){
        pNum->u.s32.u[i] = filler;
    }
}

// **************************************************
// ������UINT LargeNumDivint32(
//				PLARGENUM   pNum,
//				int       divisor,
//				PLARGENUM   pResult
//				)
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
//	IN divisor - ����
// 	OUT pResult - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	��������
// ����������
//	pResult = pNum / divisor
// ����: 
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
		//����
        divisor = -divisor;
        sd = TRUE;
    } else if(divisor == 0){
		//�� 0 ��
        //
        // This is a divide-by-zero error
        //
        for(i=0; i<SIZE_OF_LARGENUM; i++){
            pResult->u.s32.u[i] = 0xffffffff;
        }
        return 0xffffffff;
    } else {
		//�� 0 ����
        sd = FALSE;
    }
	//��λ��
    r = 0;
    for(i=(2*SIZE_OF_LARGENUM-1); i>=0; i--){
        d = (r << 16) + s[i];
        q = d / divisor;
        r = d - q * divisor;
        s[i] = q;
    }
	//������
    for(i=0; i<2*SIZE_OF_LARGENUM; i++){
        pResult->u.s16.s[i] = s[i];
    }

    if(pNum->fNegative){
		//������������  +1
        LargeNumMagInc(pResult);
		//����
        r = divisor - r;
		//�õ�����ķ���
        if(sd == 0 && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }

    } else {
		//������������
        if(sd && IsLargeNumNotZero(pResult)){
            pResult->fNegative = TRUE;
        } else {
            pResult->fNegative = FALSE;
        }
    }

    return r;
}

// **************************************************
// ������int LargeNumBits(
//				PLARGENUM   pNum
//				)
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���ش����������Чbitλ
// ����������
//	�õ������������Чbitλ
// ����: 
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
// ������char * LargeNumToAscii(
//								PLARGENUM   pNum
//								)
// ������
// 	IN pNum - LARGENUM �ṹָ�룬��������
// ����ֵ��
//	���� ascii ָ��
// ����������
//	������ת��Ϊ�ַ���
// ����: 
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
	//��Ϊ��λ������ �ַ�ָ��Ŀ�ʼ��ַ��������Ҫ��ת
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
