; �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e
; $Id: Predict_asm_x64.asm 1119 2013-12-21 06:30:46Z umezawa $


%include "Common_asm_x64.mac"


section .text


%push

global sse2_PredictLeftAndCount
sse2_PredictLeftAndCount:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, qword [rsp + %$pSrcEnd]
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	mov			rbx, qword [rsp + %$pCountTable]

	align		64
.label1:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label1

	; �ŏ��̃��C����16�o�C�g�ɖ����Ȃ���������������B
	; �኱�̂͂ݏo���ǂݍ��݂���������B
	mov			rax, qword [rsp + %$pSrcEnd]
	cmp			rsi, rax
	jae			.label4

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	add			rsi, 1
	add			rdi, 1
	cmp			rsi, rax
	jb			.label3

.label4:
	SIMPLE_EPILOGUE

%pop


; prediction �͑O��Ƀ}�[�W�������� CFrameBuffer ��ōs���̂ŁA�v�Z���ʂ��ς��Ȃ�����A�͂ݏo���ǂݍ��݂͋��e�����B
; ����A�}���`�X���b�h���삵�����ɖ�肪��������̂ŁA�͂ݏo���������݂͋��e����Ȃ��B

%push

global sse2_PredictWrongMedianAndCount
sse2_PredictWrongMedianAndCount:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, rsi
	mov			rdx, qword [rsp + %$dwStride]
	and			rdx, 0fffffffffffffff0h
	add			rax, rdx
	mov			rbx, qword [rsp + %$pCountTable]

	; �ŏ��̃��C����16�o�C�g����������B
	align		64
.label1:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label1

	; �ŏ��̃��C����16�o�C�g�ɖ����Ȃ���������������B
	; �኱�̂͂ݏo���ǂݍ��݂���������B
	mov			rax, qword [rsp + %$pSrcBegin]
	add			rax, qword [rsp + %$dwStride]
	cmp			rsi, rax
	jae			.label4

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	add			rsi, 1
	add			rdi, 1
	cmp			rsi, rax
	jb			.label3

	; �c��̃��C����16�o�C�g����������B
.label4:
	mov			rdx, qword [rsp + %$dwStride]
	neg			rdx
	mov			rax, qword [rsp + %$pSrcEnd]
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
.label2:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqu		xmm4, oword [rsi+rdx]
	movdqa		xmm6, xmm4
	pslldq		xmm6, 1
	por			xmm6, xmm5
	movdqa		xmm5, xmm4
	psrldq		xmm5, 15

	movdqa		xmm7, xmm2
	paddb		xmm7, xmm4
	psubb		xmm7, xmm6

	movdqa		xmm3, xmm2
	pminub		xmm2, xmm4
	pmaxub		xmm3, xmm4
	pmaxub		xmm7, xmm2
	pminub		xmm7, xmm3	; predicted = min(max(min(left, above), grad), max(left, above))

	psubb		xmm0, xmm7
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label2

	; �c��̃��C����16�o�C�g�ɖ����Ȃ���������������B
	; �኱�̂͂ݏo���ǂݍ��݂���������B
	mov			rax, qword [rsp + %$pSrcEnd]
	cmp			rsi, rax
	jae			.label6

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1

	movdqu		xmm4, oword [rsi+rdx]
	movdqa		xmm6, xmm4
	pslldq		xmm6, 1
	por			xmm6, xmm5

	movdqa		xmm7, xmm2
	paddb		xmm7, xmm4
	psubb		xmm7, xmm6

	movdqa		xmm3, xmm2
	pminub		xmm2, xmm4
	pmaxub		xmm3, xmm4
	pmaxub		xmm7, xmm2
	pminub		xmm7, xmm3	; predicted = min(max(min(left, above), grad), max(left, above))

	psubb		xmm0, xmm7

.label5:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	add			rsi, 1
	add			rdi, 1
	cmp			rsi, rax
	jb			.label5

.label6:
	SIMPLE_EPILOGUE

%pop


; p{min,max}ub �� SSE1 �Œǉ����ꂽ MMX ���߁i������ MMX2 ���߁j�ł���B

%push

global sse1mmx_RestoreWrongMedian
sse1mmx_RestoreWrongMedian:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, rsi
	mov			rbp, qword [rsp + %$dwStride]
	add			rax, rbp
	neg			rbp

	mov			edx, 80h

	align		64
.label1:
	add			dl, byte [rsi]
	mov			byte [rdi], dl
	add 		rsi, 1
	add			rdi, 1
	cmp			rsi, rax
	jb			.label1

	pxor		mm4, mm4
	pxor		mm2, mm2

	align		64
.label2:
	movq		mm6, mm2
	movq		mm7, mm2
	psubb		mm6, mm4
	movd		mm4, dword [rdi+rbp]		; mm4 = above
	paddb		mm6, mm4					; mm6 = grad

	pminub		mm2, mm6
	pmaxub		mm6, mm7
	pmaxub		mm2, mm4
	pminub		mm2, mm6					; mm2 = median

	paddb		mm2, qword [rsi]		; �A���C�����g������Ă��Ă� xmm ���W�X�^�̏ꍇ�ƈ���Ĉ�ʕی��O�ɂ͂Ȃ�Ȃ�
	movd		eax, mm2
	mov			byte [rdi], al

	add			rsi, 1
	add			rdi, 1
	cmp			rsi, qword [rsp + %$pSrcEnd]
	jb			.label2

	emms
	SIMPLE_EPILOGUE

%pop


%push
global sse2_RestoreWrongMedianBlock4
sse2_RestoreWrongMedianBlock4:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pDstBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			rbx, [rsp + %$cbWidth]
	mov			rcx, [rsp + %$scbStride]
	mov			rax, rcx
	sub			rcx, rbx
	neg			rax

	mov			edx, 80808080h
	movd		xmm0, edx

	lea			rdx, [rsi+rbx]
.label0:
	movd		xmm1, [rsi]
	paddb		xmm0, xmm1
	movd		[rdi], xmm0
	add 		rsi, 4
	add			rdi, 4
	cmp			rsi, rdx
	jb			.label0

	add			rsi, rcx
	add			rdi, rcx
	cmp			rsi, r9
	je			.label4

	pxor		xmm4, xmm4
	pxor		xmm2, xmm2

.label1:
	lea			rdx, [rsi+rbx]
.label3:
	movdqa		xmm6, xmm2
	movdqa		xmm7, xmm2
	psubb		xmm6, xmm4
	movd		xmm4, [rdi+rax]		; mm4 = above
	paddb		xmm6, xmm4			; mm6 = grad

	pminub		xmm2, xmm6
	pmaxub		xmm6, xmm7
	pmaxub		xmm2, xmm4
	pminub		xmm2, xmm6			; mm2 = median

	movd		xmm0, [rsi]
	paddb		xmm2, xmm0
	movd		[rdi], xmm2
	add			rsi, 4
	add			rdi, 4
	cmp			rsi, rdx
	jb			.label3

.label2:
	add			rsi, rcx
	add			rdi, rcx
	cmp			rsi, r9
	jne			.label1

.label4:
	SIMPLE_EPILOGUE
%pop
