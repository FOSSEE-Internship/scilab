/*
* Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
* Copyright (C) 2011 - DIGITEO - Cedric DELAMARRE
*
* This file must be used under the terms of the CeCILL.
* This source file is licensed as described in the file COPYING, which
* you should have received as part of this distribution.  The terms
* are also available at
* http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
*
*/
/*--------------------------------------------------------------------------*/

#include "linear_algebra_gw.hxx"
#include "function.hxx"
#include "double.hxx"
#include "overload.hxx"
#include "execvisitor.hxx"

extern "C"
{
#include "localization.h"
#include "Scierror.h"
#include "lsq.h"
}
/*--------------------------------------------------------------------------*/

types::Function::ReturnValue sci_lsq(types::typed_list &in, int _iRetCount, types::typed_list &out)
{
    types::Double* pDbl[2]      = {NULL, NULL};
    types::Double* pDblResult   = NULL;
    double* pData[2]            = {NULL, NULL};
    double* pResult             = NULL;
    double* pdTol               = NULL;
    bool bComplexArgs           = false;
    int iRank                   = 0;

    if(in.size() < 2 || in.size() > 3)
    {
        ScierrorW(77, _W("%ls: Wrong number of input argument(s): %d to %d expected.\n"), L"lsq", 2, 3);
        return types::Function::Error;
    }

    if(_iRetCount > 2)
    {
        ScierrorW(78, _W("%ls: Wrong number of output argument(s): %d to %d expected.\n"), L"lsq", 1, 2);
        return types::Function::Error;
    }

    if((in[0]->isDouble() == false))
    {
        std::wstring wstFuncName = L"%"  + in[0]->getShortTypeStr() + L"_lsq";
        return Overload::call(wstFuncName, in, _iRetCount, out, new ExecVisitor());
    }

    if(in.size() == 2)
    {
        if((in[1]->isDouble() == false))
        {
            std::wstring wstFuncName = L"%"  + in[1]->getShortTypeStr() + L"_lsq";
            return Overload::call(wstFuncName, in, _iRetCount, out, new ExecVisitor());
        }
        pDbl[1] = in[1]->getAs<types::Double>()->clone()->getAs<types::Double>();
    }

    if(in.size() == 3)
    {
        if((in[2]->isDouble() == false) || (in[2]->getAs<types::Double>()->isComplex()) || (in[2]->getAs<types::Double>()->isScalar() == false))
        {
            ScierrorW(256, _W("%ls: Wrong type for input argument #%d: A Real expected.\n"), L"lsq", 3);
            return types::Function::Error;     
        }
        *pdTol = in[2]->getAs<types::Double>()->get(0);
    }

    pDbl[0] = in[0]->getAs<types::Double>()->clone()->getAs<types::Double>();

    if(pDbl[0]->getRows() != pDbl[1]->getRows())
    {
        ScierrorW(265, _W("%ls: %ls and %ls must have equal number of rows.\n"), L"lsq", L"A", L"B");
        return types::Function::Error;
    }

    if((pDbl[0]->getCols() == 0) || (pDbl[1]->getCols() == 0))
    {
        out.push_back(types::Double::Empty());
        if(_iRetCount == 2)
        {
            out.push_back(types::Double::Empty());
        }
        return types::Function::OK;
    }

    if(pDbl[0]->isComplex() || pDbl[1]->isComplex())
    {
        bComplexArgs = true;
    }
    for(int i=0; i<2; i++)
    {
        if(pDbl[i]->getCols() == -1)
        {
            ScierrorW(271, _W("%ls: Size varying argument a*eye(), (arg %d) not allowed here.\n"), L"lsq", i+1);
            return types::Function::Error;
        }

        if(bComplexArgs)
        {
            pData[i] = (double*)oGetDoubleComplexFromPointer(pDbl[i]->getReal(), pDbl[i]->getImg(), pDbl[i]->getSize());
            if(!pData[i])
            {
                ScierrorW(999,_W("%ls: Cannot allocate more memory.\n"),L"lsq");
                return types::Function::Error;
            }
        }
        else
        {
            pData[i] = pDbl[i]->getReal();
        }
    }

    pDblResult = new types::Double(pDbl[0]->getCols(), pDbl[1]->getCols(), bComplexArgs);

    if(bComplexArgs)
    {
        pResult = (double*)MALLOC(pDbl[0]->getCols() * pDbl[1]->getCols() * sizeof(doublecomplex));
    }
    else
    {
        pResult = pDblResult->get();
    }

    int iRet = iLsqM(pData[0], pDbl[0]->getRows(), pDbl[0]->getCols(), pData[1], pDbl[1]->getCols(), bComplexArgs, pResult, pdTol, ((_iRetCount == 2) ? &iRank : NULL));

    if(iRet != 0)
    {
        if(iRet == -1)
        {
		    ScierrorW(999, _W("%ls: Allocation failed.\n"), L"lsq");
        }
        else
        {
		    ScierrorW(999, _W("%ls: LAPACK error n°%d.\n"), L"lsq",iRet);
        }
        return types::Function::Error;
    }

    if(bComplexArgs)
    {
        vGetPointerFromDoubleComplex((doublecomplex*)(pResult), pDblResult->getSize(), pDblResult->getReal(), pDblResult->getImg());
        vFreeDoubleComplexFromPointer((doublecomplex*)pResult);
		vFreeDoubleComplexFromPointer((doublecomplex*)pData[0]);
        vFreeDoubleComplexFromPointer((doublecomplex*)pData[1]);
    }

    out.push_back(pDblResult);
    if(_iRetCount == 2)
    {
        types::Double* pDblRank = new types::Double(1,1);
        pDblRank->set(0, iRank);
        out.push_back(pDblRank);
    }

    return types::Function::OK;
}
/*--------------------------------------------------------------------------*/
