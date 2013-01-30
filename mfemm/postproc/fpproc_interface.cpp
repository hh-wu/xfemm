//#include "xyplot.h"
//#include "femmviewDoc.h"
//#include "femmviewview.h"
//#include "MainFrm.h"
//#include "windows.h" //necessary for bitmap saving
//#include "lua.h"
//#include "mycommandlineinfo.h"
//#include "promptbox.h"
//#include "ChildFrm.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//extern lua_State *lua;
//extern BOOL bLinehook;
//extern void *pFemmviewdoc;
//extern CLuaConsoleDlg *LuaConsole;
//extern BOOL lua_byebye;
//extern int m_luaWindowStatus;

#include <iostream>
#include <string>
#include <cstring>
#include "mex.h"
#include "fpproc.h"
#include "problem.h"
#include "fpproc_interface.h"
//#include "femm.h"

using namespace std;

//////////////////      MEX Interface        //////////////////////

FPProc_interface::FPProc_interface()
{

}

//FPProc_interface::FPProc_interface(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
//{
//    theFPProc.OpenDocument(filename);
//}

int FPProc_interface::opendocument(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    char *input_buf;
    double *outpointer;
    int result;
    string filename;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 0)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    /* 3rd input must be a string (first two are used for the class interface) */
    if ( mxIsChar(prhs[2]) != 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:inputNotString",
                           "Input must be a string containing the file name.");

    /* input must be a row vector */
    if (mxGetM(prhs[2])!=1)
        mexErrMsgIdAndTxt( "MATLAB:revord:inputNotVector",
                           "Input must be a row vector.");

    /* copy the string data from prhs[2] into a C string input_ buf.    */
    input_buf = mxArrayToString(prhs[2]);
    // create a std::string from the input C string
    filename = input_buf;

    // call the OpenDocument method with the input
    result = theFPProc.OpenDocument(filename);

    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = (double)(result);

    return result;
}

int FPProc_interface::getpointvals(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *px, *py, *outpointerRe, *outpointerIm;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);

    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    if((nxcols>1) | (nycols>1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "x and y must both be column vectors.");
    }

    if(myrows != myrows)
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "x and y must be column vectors of the same size.");
    }

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(14), (mwSize)mxrows, mxCOMPLEX);
    // get a pointer to the start of the actual output data array
    outpointerRe = mxGetPr(plhs[0]);
    outpointerIm = mxGetPi(plhs[0]);

    for(int i=0; i<(int)mxrows; i++)
    {
        CPointVals u;

        if(theFPProc.GetPointValues(px[i], py[i], u)==TRUE)
        {
            outpointerRe[(i*23)] = u.A.Re();
            outpointerIm[(i*14)] = u.A.Im();
            outpointerRe[(i*14)+1] = u.B1.Re();
            outpointerIm[(i*14)+1] = u.B1.Im();
            outpointerRe[(i*14)+2] = u.B2.Re();
            outpointerIm[(i*14)+2] = u.B2.Im();
            outpointerRe[(i*14)+3] = u.c;
            outpointerRe[(i*14)+4] = u.E;
            outpointerRe[(i*14)+5] = u.H1.Re();
            outpointerIm[(i*14)+5] = u.H1.Im();
            outpointerRe[(i*14)+6] = u.H2.Re();
            outpointerIm[(i*14)+6] = u.H2.Im();
            outpointerRe[(i*14)+7] = u.Je.Re();
            outpointerIm[(i*14)+7] = u.Je.Im();
            outpointerRe[(i*14)+8] = u.Js.Re();
            outpointerIm[(i*14)+8] = u.Js.Im();
            outpointerRe[(i*14)+9] = u.mu1.Re();
            outpointerIm[(i*14)+9] = u.mu1.Im();
            outpointerRe[(i*14)+10] = u.mu2.Re();
            outpointerIm[(i*14)+10] = u.mu2.Im();
            outpointerRe[(i*14)+11] = u.Pe;
            outpointerRe[(i*14)+12] = u.Ph;
            outpointerRe[(i*14)+13] = u.ff;
        }
        else
        {
            return 0;
        }
    }

    return 14;
}


int FPProc_interface::addcontour(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    CComplex z;
    int nconpts;
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "Two inputs required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the no. of rows of the matrix input x and y */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);
    /*  get the no. of cols of the matrix input x and y */
    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    // check dimensions are allowed
    if((nxcols>1) | (nycols>1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "x and y must both be row vectors.");
    }

    if(mxrows != myrows)
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "x and y must be row vectors of the same size.");
    }

    for (int ind=0; ind<mxrows; ind++)
    {

        z.Set(px[ind],py[ind]);

        nconpts = theFPProc.contour.size();

        if(nconpts>0)
        {
            if (z != theFPProc.contour[nconpts-1])
            {
                theFPProc.contour.push_back(z);
            }
        }
        else
        {
            theFPProc.contour.push_back(z);
        }

    }

    // now return the actual points in the current contour
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(theFPProc.contour.size()), (mwSize)(2), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    nconpts = theFPProc.contour.size();
    for (int ind=0; ind<nconpts; ind++)
    {
        outpointer[ind] = theFPProc.contour[ind].re;
        outpointer[ind+nconpts] = theFPProc.contour[ind].im;
    }

    return 0;
}

int FPProc_interface::clearcontour()
{
    theFPProc.contour.clear();

    return 0;
}

int FPProc_interface::lineintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;
    CComplex *z;
    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];
    // 0- B.n
    // 1 - H.t
    // 2 - Cont length
    // 3 - Force from stress tensor
    // 4 - Torque from stress tensor
    // 5 - (B.n)^2

    if (type<0 || type >5)
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidinttype",
                           "Invalid line integral selected %d",type);
        return 0;
    }

    // Create an array of CComplex to hold the results of the integral
    z = (CComplex *)calloc(4,sizeof(CComplex));
    theFPProc.LineIntegral(type,z);

    switch(type)
    {
    case 2: // length result
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = z[0].re; // contour length
        outpointerRe[1] = z[0].im; // swept area
        free(z);
        return 2;

    case 3: // force results
        if (theFPProc.Frequency!=0)
        {
            // Stress Tensor Force
            // 1: DC r/x force
            // 2: DC y/z force
            // 3: 2× r/x force
            // 4: 2× y/z force

            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(4), (mwSize)(1), mxCOMPLEX );
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            outpointerRe[0] = z[2].re; //outpointerIm[0] = 0.0;
            outpointerRe[1] = z[3].re; //outpointerIm[1] = 0.0;
            outpointerRe[2] = z[0].re;
            outpointerIm[2] = z[0].im;
            outpointerRe[3] = z[1].re;
            outpointerIm[3] = z[1].im;

            free(z);
            return 4;
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerRe[0] = z[0].re;
            outpointerRe[1] = z[1].re;

            free(z);
            return 2;
        }


    case 4: // torque results
        if(theFPProc.Frequency!=0)
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerIm = mxGetPi(plhs[0]);
            outpointerRe[0] = z[1].re;
            outpointerRe[1] = z[0].re;
            outpointerIm[1] = z[0].im;

            free(z);
            return 2;
        }
        else
        {
            /*  set the output pointer to the output matrix */
            plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxREAL);
            // get a pointer to the start of the actual output data array
            outpointerRe = mxGetPr(plhs[0]);
            outpointerRe[0] = z[0].re;

            free(z);
            return 2;
        }

    default:
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(2), (mwSize)(1), mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        outpointerRe = mxGetPr(plhs[0]);
        outpointerIm = mxGetPi(plhs[0]);
        outpointerRe[0] = z[0].re;
        outpointerIm[0] = z[0].im;
        outpointerRe[1] = z[1].re;
        outpointerIm[1] = z[1].im;

        free(z);
        return 2;
    }

    free(z);
    return 0;
}

int FPProc_interface::selectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    //CatchNullDocument();//
    double *px, *py, *outpointer;
    size_t mxrows, myrows, nxcols, nycols;

    /* check for proper number of arguments */
    if(nrhs!=4)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    px = mxGetPr(prhs[2]);
    py = mxGetPr(prhs[3]);

    /*  get the dimensions of the matrix input x */
    mxrows = mxGetM(prhs[2]);
    myrows = mxGetM(prhs[3]);

    nxcols = mxGetN(prhs[2]);
    nycols = mxGetN(prhs[3]);
    if((mxrows != 1) | (myrows != 1) | (nxcols != 1) | (nycols != 1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "x and y must both be scalars.");
    }

    int k;

    if (theFPProc.meshelem.size()>0)
    {
        k = theFPProc.InTriangle(px[0],py[0]);

        if(k>=0)
        {
            theFPProc.bHasMask = FALSE;
            theFPProc.blocklist[theFPProc.meshelem[k].lbl].ToggleSelect();
        }
    }

    return 0;
}

int FPProc_interface::groupselectblock(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int j,k,n;
    double *pgroup;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "group number must be a scalar.");
    }

    // get a pointer to the actual data
    pgroup = mxGetPr(prhs[2]);

    if (theFPProc.meshelem.size()>0)
    {
        k = 0;

        if (pgroup[0] > 0)
        {
            k = (int)pgroup[0];
        }

        for(j=0; j<theFPProc.blocklist.size(); j++)
        {
            if ((theFPProc.blocklist[j].InGroup==k) || (pgroup[0]==0))
            {
                theFPProc.blocklist[j].ToggleSelect();
            }

            theFPProc.bHasMask = FALSE;
        }
    }

    return 0;
}

int FPProc_interface::clearblock()
{
    theFPProc.bHasMask = FALSE;

    for(int i=0; i<theFPProc.blocklist.size(); i++)
    {
        if (theFPProc.blocklist[i].IsSelected == TRUE)
        {
            theFPProc.blocklist[i].IsSelected = FALSE;
        }
    }

    return 0;
}


int FPProc_interface::blockintegral(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int type;

    double *ptype, *outpointerRe, *outpointerIm;
    size_t mrows, ncols;

    /* check for proper number of arguments */
    if(nrhs!=3)
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidNumInputs",
                           "One input required.");
    else if(nlhs > 1)
        mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
                           "Too many output arguments.");

    /*  get the dimensions of the matrix input x */
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    // check dimensions are allowed
    if((mrows!=1) | (ncols!=1))
    {
        mexErrMsgIdAndTxt( "MATLAB:revord:invalidSizeInputs",
                           "type must be a scalar.");
    }

    // get a pointer to the actual data
    ptype = mxGetPr(prhs[2]);

    // cast the value in the matlab matrix to an integer
    type = (int) ptype[0];

    CComplex z;

    BOOL flg = FALSE;
    for(unsigned int i=0; i<theFPProc.blocklist.size(); i++)
    {
        if (theFPProc.blocklist[i].IsSelected==TRUE)
        {
            flg = TRUE;
        }
    }

    if(flg==FALSE)
    {
        mexErrMsgIdAndTxt("MATLAB:revord:noblockselected",
                          "Block integral failed, no area has been selected");
    }

    if((type>=18) && (type<=23))
    {
        theFPProc.MakeMask();
    }

    z = theFPProc.BlockIntegral(type);

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(1), (mwSize)(1), mxCOMPLEX);
    // get a pointer to the start of the actual output data array
    outpointerRe = mxGetPr(plhs[0]);
    outpointerIm = mxGetPr(plhs[0]);
    outpointerRe[0] = z.re;
    outpointerIm[0] = z.im;

    return 1;
}


int FPProc_interface::smoothon()
{
    theFPProc.Smooth = TRUE;
    return 0;
}

int FPProc_interface::smoothoff()
{
    theFPProc.Smooth = FALSE;
    return 0;
}

int FPProc_interface::getprobleminfo(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *outpointer;

    /*  set the output pointer to the output matrix */
    plhs[0] = mxCreateDoubleMatrix( (mwSize)(4), (mwSize)(1), mxREAL);
    // get a pointer to the start of the actual output data array
    outpointer = mxGetPr(plhs[0]);
    outpointer[0] = theFPProc.ProblemType;
    outpointer[1] = theFPProc.Frequency;
    outpointer[2] = theFPProc.Depth;
    outpointer[3] = theFPProc.LengthConv[theFPProc.LengthUnits];

    return 4;
}

int FPProc_interface::getcircuitprops(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int NumCircuits,k;
    char circuitname[1024];
    k=-1;

    if (nrhs < 3 || mxGetString(prhs[2], circuitname, sizeof(circuitname)))
    {
        mexErrMsgTxt("Input should be a string containng a circuit name less than 1024 characters long.");
    }

    // ok we need to find the correct entry for the circuit name
    NumCircuits = theFPProc.circproplist.size();

    for(int i=0; i<NumCircuits; i++)
    {
        if(strcmp(theFPProc.circproplist[i].CircName.c_str(), circuitname))
        {
            k = i;
            i = NumCircuits; // that will break it
        }
    }

// trap errors

    if(k==-1)
    {
        mexErrMsgTxt("Unknown circuit");
    }

    CComplex amps,volts,fluxlinkage;

    amps = theFPProc.circproplist[k].Amps;
    volts = theFPProc.GetVoltageDrop(k);
    fluxlinkage = theFPProc.GetFluxLinkage(k);

    if (theFPProc.Frequency != 0)
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(3), (mwSize)(1), mxCOMPLEX);
        // get a pointer to the start of the actual output data array
        double *outpointerRe = mxGetPr(plhs[0]);
        double *outpointerIm = mxGetPi(plhs[0]);
        outpointerRe[0] = amps.re;
        outpointerIm[0] = amps.im;
        outpointerRe[1] = volts.re;
        outpointerIm[1] = volts.im;
        outpointerRe[2] = fluxlinkage.re;
        outpointerIm[2] = fluxlinkage.im;
    }
    else
    {
        /*  set the output pointer to the output matrix */
        plhs[0] = mxCreateDoubleMatrix( (mwSize)(3), (mwSize)(1), mxREAL);
        // get a pointer to the start of the actual output data array
        double *outpointerRe = mxGetPr(plhs[0]);
        outpointerRe[0] = amps.re;
        outpointerRe[1] = volts.re;
        outpointerRe[2] = fluxlinkage.re;
    }

    return 3;
}


///////////////////////////////////////////////////////////////
/////////////      FUNCTIONS TO BE CONVERTED     //////////////
///////////////////////////////////////////////////////////////


//
//
//int FPProc_interface::selectline(lua_State *L)
//{
//		//CatchNullDocument();//
//        FPProc_interface * pDoc; // note normally thisdoc
//        CFemmviewView * theView;
//        pDoc=(FPProc_interface *)pFemmviewdoc;
//        POSITION pos;
//        pos=pDoc->GetFirstViewPosition();
//        theView=(CFemmviewView *)pDoc->GetNextView(pos);
//
//        theView->EditAction=1; // make sure things update OK
//
//        double mx,my;
//        int i,j,k,m;
//
//        mx=lua_todouble(L,1);
//        my=lua_todouble(L,2);
//
//
////***************
//        {
//                if (pDoc->nodelist.GetSize()>0){
//                        i=pDoc->ClosestNode(mx,my);
//                        CComplex x,y,z;
//                        double R,d1,d2;
//                        int lineno,arcno,flag=0;
//                        z.Set(pDoc->nodelist[i].x,pDoc->nodelist[i].y);
//
//                        if (pDoc->contour.GetSize()>0){
//
//                                //check to see if point is the same as last point in the contour;
//                                y=pDoc->contour[pDoc->contour.GetSize()-1];
//                                if ((y.re==z.re) && (y.im==z.im)) return 0;
//
//                                j=pDoc->ClosestNode(y.re,y.im);
//                                x.Set(pDoc->nodelist[j].x,pDoc->nodelist[j].y);
//
//                                //check to see if this point and the last point are ends of an
//                                //input segment;
//                                lineno=-1;
//                                d1=1.e08;
//
//                                if (abs(x-y)<1.e-08){
//                                        for(k=0;k<pDoc->linelist.GetSize();k++){
//                                                if((pDoc->linelist[k].n0==j) && (pDoc->linelist[k].n1==i))
//                                                {
//                                                        d2=fabs(pDoc->ShortestDistanceFromSegment(mx,my,k));
//                                                        if(d2<d1){
//                                                                lineno=k;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                                if((pDoc->linelist[k].n0==i) && (pDoc->linelist[k].n1==j))
//                                                {
//                                                        d2=fabs(pDoc->ShortestDistanceFromSegment(mx,my,k));
//                                                        if(d2<d1){
//                                                                lineno=k;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                        }
//                                }
//
//                                //check to see if this point and last point are ends of an
//                                // arc segment; if so, add entire arc to the contour;
//                                arcno=-1;
//                                if (abs(x-y)<1.e-08){
//                                        for(k=0;k<pDoc->arclist.GetSize();k++){
//                                                if((pDoc->arclist[k].n0==j) && (pDoc->arclist[k].n1==i))
//                                                {
//                                                        d2=pDoc->ShortestDistanceFromArc(CComplex(mx,my),
//                                                           pDoc->arclist[k]);
//                                                        if(d2<d1){
//                                                                arcno=k;
//                                                                lineno=-1;
//                                                                flag=TRUE;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                                if((pDoc->arclist[k].n0==i) && (pDoc->arclist[k].n1==j))
//                                                {
//                                                        d2=pDoc->ShortestDistanceFromArc(CComplex(mx,my),
//                                                           pDoc->arclist[k]);
//                                                        if(d2<d1){
//                                                                arcno=k;
//                                                                lineno=-1;
//                                                                flag=FALSE;
//                                                                d1=d2;
//                                                        }
//                                                }
//                                        }
//                                }
//
//                        }
//                        else{
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(FALSE);
//                                return 0;
//                        }
//
//                        if((lineno<0) && (arcno<0)){
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(FALSE);
//                        }
//
//                        if(lineno>=0){
//                                j=pDoc->contour.GetSize();
//                                if(j>1){
//                                        if(abs(pDoc->contour[j-2]-z)<1.e-08){
//                                                //CView::OnLButtonDown(nFlags, point);
//                                                return 0;
//                                        }
//                                }
//                                pDoc->contour.Add(z);
//                                theView->DrawUserContour(FALSE);
//                        }
//                        if(arcno>=0){
//                                k=arcno;
//                                pDoc->GetCircle(pDoc->arclist[k],x,R);
//                                j=(int) ceil(pDoc->arclist[k].ArcLength/pDoc->arclist[k].MaxSideLength);
//                                if(flag==TRUE)
//                                        z=exp(I*pDoc->arclist[k].ArcLength*PI/(180.*((double) j)) );
//                                else
//                                        z=exp(-I*pDoc->arclist[k].ArcLength*PI/(180.*((double) j)) );
//                                for(i=0;i<j;i++){
//                                        y=(y-x)*z+x;
//                                        m=pDoc->contour.GetSize();
//                                        if(m>1){
//                                                if(abs(pDoc->contour[m-2]-y)<1.e-08){
//                                                        //CView::OnLButtonDown(nFlags, point);
//                                                        return 0;
//                                                }
//                                        }
//                                        pDoc->contour.Add(y);
//                                        theView->DrawUserContour(FALSE);
//                                }
//                        }
//                }
//        }
//
//
////*************
//        return 0;
//}
//
//int FPProc_interface::seteditmode(lua_State *L)
//{
//    //CatchNullDocument();//
//	CString EditAction;
//
//    EditAction.Format("%s",lua_tostring(L,1));
//    EditAction.MakeLower();
//
//	FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	    if (EditAction=="point")
//		{
//			if (theView->EditAction==1){
//				theView->EraseUserContour(TRUE);
//				thisDoc->contour.RemoveAll();
//			}
//			if (theView->EditAction==2){
//				int i;
//				BOOL flg=FALSE;
//				thisDoc->bHasMask=FALSE;
//				for(i=0;i<thisDoc->blocklist.GetSize();i++)
//				{
//					if (thisDoc->blocklist[i].IsSelected==TRUE)
//					{
//						thisDoc->blocklist[i].IsSelected=FALSE;
//						flg=TRUE;
//					}
//				}
//				if(flg==TRUE) theView->RedrawView();
//			}
//			theView->EditAction=0;
//	}
//    if (EditAction=="contour")
//	{
//		if(theView->EditAction==2){
//			int i;
//			BOOL flg=FALSE;
//			thisDoc->bHasMask=FALSE;
//			for(i=0;i<thisDoc->blocklist.GetSize();i++){
//				if (thisDoc->blocklist[i].IsSelected==TRUE)
//				{
//					thisDoc->blocklist[i].IsSelected=FALSE;
//					flg=TRUE;
//				}
//			}
//
//			if(flg==TRUE) theView->RedrawView();
//		}
//		theView->EditAction=1;
//	}
//    if (EditAction=="area")
//	{
//		if(theView->EditAction==1){
//			theView->EraseUserContour(TRUE);
//			thisDoc->contour.RemoveAll();
//		}
//		theView->EditAction=2;
//	}
//
//	return 0;
//}
//
//int FPProc_interface::bendcontour(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	thisDoc->BendContour(lua_todouble(L,1),lua_todouble(L,2));
//	theView->InvalidateRect(NULL);
//
//	return 0;
//}


///*
//int FPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//    double x,y;
//    int i,n[3];
//    double b[3],c[3],da;
//    CComplex dbxdx,dbxdy,dbydx,dbydy;
//    CElement elm;
//
//    x=lua_todouble(L,1);
//    y=lua_todouble(L,2);
//    i=thisDoc->InTriangle(x,y);
//    if(i<0){
//        for(i=0;i<8;i++) lua_pushnumber(L,0);
//        return 8;
//    }
//
//    elm=thisDoc->meshelem[i];
//    for(i=0;i<3;i++) n[i]=elm.p[i];
//    b[0]=thisDoc->meshnode[n[1]].y - thisDoc->meshnode[n[2]].y;
//    b[1]=thisDoc->meshnode[n[2]].y - thisDoc->meshnode[n[0]].y;
//    b[2]=thisDoc->meshnode[n[0]].y - thisDoc->meshnode[n[1]].y;
//    c[0]=thisDoc->meshnode[n[2]].x - thisDoc->meshnode[n[1]].x;
//    c[1]=thisDoc->meshnode[n[0]].x - thisDoc->meshnode[n[2]].x;
//    c[2]=thisDoc->meshnode[n[1]].x - thisDoc->meshnode[n[0]].x;
//    da=(b[0]*c[1]-b[1]*c[0])*thisDoc->LengthConv[thisDoc->LengthUnits];
//
//	dbxdx=0; dbxdy=0; dbydx=0; dbydy=0;
//    for(i=0;i<3;i++)
//    {
//		dbxdx+=elm.b1[i]*b[i]/da;
//		dbxdy+=elm.b1[i]*c[i]/da;
//		dbydx+=elm.b2[i]*b[i]/da;
//		dbydy+=elm.b2[i]*c[i]/da;
//	}
//
//	if (thisDoc->ProblemType==PLANAR)
//	{
//		// in a source-free region, we can get a little bit
//		// of smoothing by enforcing the dependencies that
//		// are implied by the differential equation.
//		dbxdx=(dbxdx-dbydy)/2.;
//		dbydy=-dbxdx;
//		dbydx=(dbydx+dbxdy)/2;
//		dbxdy=dbydx;
//	}
//
//    lua_pushnumber(L,Re(dbxdx));
//    lua_pushnumber(L,Im(dbxdx));
//	lua_pushnumber(L,Re(dbxdy));
//    lua_pushnumber(L,Im(dbxdy));
//	lua_pushnumber(L,Re(dbydx));
//    lua_pushnumber(L,Im(dbydx));
//	lua_pushnumber(L,Re(dbydy));
//    lua_pushnumber(L,Im(dbydy));
//
//    return 8;
//}
//*/
//
//int FPProc_interface::gradient(lua_State *L)
//{
//	// computes the gradients of the B field by differentiating
//	// the shape functions that are used to represent the smoothed
//	// B in an element.
//
//    //CatchNullDocument();//
//    FPProc_interface * thisDoc;
//    CFemmviewView * theView;
//    thisDoc=(FPProc_interface *)pFemmviewdoc;
//    POSITION pos;
//    pos=thisDoc->GetFirstViewPosition();
//    theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//    int i;
//	double xo,yo,x,y,da,p0,p1,p2;
//    CComplex dbxdx,dbxdy,dbydx,dbydy,Mx,My;
//
//    xo=lua_todouble(L,1);
//    yo=lua_todouble(L,2);
//
//	dbxdx=0;
//	dbxdy=0;
//	dbydx=0;
//	dbydy=0;
//
//	for(i=0;i<thisDoc->meshelem.GetSize();i++)
//	{
//		thisDoc->GetMagnetization(i,Mx,My);
//		da=muo*thisDoc->ElmArea(i)/thisDoc->LengthConv[thisDoc->LengthUnits];
//		x=Re(thisDoc->meshelem[i].ctr);
//		y=Im(thisDoc->meshelem[i].ctr);
//
//		p0=PI*pow(pow(x - xo,2.) + pow(y - yo,2.),3.);
//		p1=(-3.*pow(x - xo,2.) + pow(y - yo,2.))*(y - yo);
//		p2=(x - xo)*(pow(x - xo,2.) - 3.*pow(y - yo,2.));
//		dbxdx+=(da*(-(My*p1) + Mx*p2))/p0;
//		dbydx+=(da*(-(Mx*p1) - My*p2))/p0;
//		dbxdy+=(da*(-(Mx*p1) - My*p2))/p0;
//		dbydy+=-(da*(-(My*p1) + Mx*p2))/p0;
//	}
//
//    lua_pushnumber(L,Re(dbxdx));
//    lua_pushnumber(L,Im(dbxdx));
//	lua_pushnumber(L,Re(dbxdy));
//    lua_pushnumber(L,Im(dbxdy));
//	lua_pushnumber(L,Re(dbydx));
//    lua_pushnumber(L,Im(dbydx));
//	lua_pushnumber(L,Re(dbydy));
//    lua_pushnumber(L,Im(dbydy));
//
//    return 8;
//}


////////////////////////////////////////////////////////////////////////////////////
//
// old versions for compatibility
//
////////////////////////////////////////////////////////////////////////////////////

//int FPProc_interface::old_lua_lineintegral(lua_State * L)
//{
//	//CatchNullDocument();//
//	int type;
//	CComplex *z;
//	z=(CComplex *)calloc(4,sizeof(CComplex));
//
//	type=(int) lua_todouble(L,1);
//	// 0- B.n
//	// 1 - H.t
//	// 2 - Cont length
//	// 3 - Force from stress tensor
//	// 4 - Torque from stress tensor
//	// 5 - (B.n)^2
//
//	if (type<0 || type >5)
//	{
//		CString msg;
//		msg.Format("Invalid line integral selected %d",type);
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//
//	}
//
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	thisDoc->LineIntegral(type,z);
//
//	switch(type)
//	{
//		case 3: // force results
//			if (thisDoc->Frequency!=0){
//				lua_pushnumber(lua,z[2].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[3].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,z[0].im);
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,z[1].im);
//				free(z);
//				return 8;
//			}
//			else{
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,0);
//				free(z);
//				return 4;
//			}
//
//
//		case 4: // torque results
//			if(thisDoc->Frequency!=0){
//				lua_pushnumber(lua,z[1].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,z[0].im);
//				free(z);
//				return 4;
//			}
//			else
//			{
//				lua_pushnumber(lua,z[0].re);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,0);
//				lua_pushnumber(lua,0);
//
//				free(z);
//				return 4;
//			}
//
//		default:
//			lua_pushnumber(lua,z[0].re);
//			lua_pushnumber(lua,z[0].im);
//
//			lua_pushnumber(lua,z[1].re);
//			lua_pushnumber(lua,z[1].im);
//
//			free(z);
//			return 4;
//	}
//
//	free(z);
//	return 0;
//}
//
//int FPProc_interface::old_lua_getpointvals(lua_State * L)
//{
//	//CatchNullDocument();//
//	double px,py;
//
//	px=lua_todouble(L,1);
//	py=lua_todouble(L,2);
//
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	CPointVals u;
//	CComplex Jtot;
//
//	if(thisDoc->GetPointValues(px, py, u)==TRUE)
//	{
//		lua_pushnumber(L,u.A.re);
//		lua_pushnumber(L,u.A.im);
//		lua_pushnumber(L,u.B1.re);
//		lua_pushnumber(L,u.B1.im);
//		lua_pushnumber(L,u.B2.re);
//		lua_pushnumber(L,u.B2.im);
//		lua_pushnumber(L,u.c);
//		lua_pushnumber(L,u.E);
//		lua_pushnumber(L,u.H1.re);
//		lua_pushnumber(L,u.H1.im);
//		lua_pushnumber(L,u.H2.re);
//		lua_pushnumber(L,u.H2.im);
//		lua_pushnumber(L,u.Je.re);
//		lua_pushnumber(L,u.Je.im);
//		lua_pushnumber(L,u.Js.re);
//		lua_pushnumber(L,u.Js.im);
//		lua_pushnumber(L,u.mu1.re);
//		lua_pushnumber(L,u.mu1.im);
//		lua_pushnumber(L,u.mu2.re);
//		lua_pushnumber(L,u.mu2.im);
//		lua_pushnumber(L,u.Pe);
//		lua_pushnumber(L,u.Ph);
//		lua_pushnumber(L,u.ff);
//		return 23;
//	}
//
//	return 0;
//}
//
//int FPProc_interface::old_lua_blockintegral(lua_State * L)
//{
//	//CatchNullDocument();//
//	int type;
//	type=(int) lua_todouble(L,1);
//
//	FPProc_interface *thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	//CatchNullDocument();//
//
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	CComplex z;
//
//	int i;
//	BOOL flg=FALSE;
//	for(i=0;i<thisDoc->blocklist.GetSize();i++)
//		if (thisDoc->blocklist[i].IsSelected==TRUE) flg=TRUE;
//	if(flg==FALSE)
//	{
//		CString msg="Cannot integrate\nNo area has been selected";
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//	}
//	if((type>=18) && (type<=23)) thisDoc->MakeMask();
//	z=thisDoc->BlockIntegral(type);
//
//	lua_pushnumber(L,z.re);
//	lua_pushnumber(L,z.im);
//
//	return 2;
//}
//
//int FPProc_interface::old_lua_getcircuitprops(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * TheDoc; // note normally thisdoc
//	CFemmviewView * theView;
//	TheDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=TheDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)TheDoc->GetNextView(pos);
//
//
//	int NumCircuits,k;
//	CString circuitname;
//	circuitname=lua_tostring(L,1);
//	k=-1;
//
//	// ok we need to find the correct entry for the circuit name
//	NumCircuits=TheDoc->circproplist.GetSize();
//	for(int i=0;i<NumCircuits;i++)
//		{
//			if(TheDoc->circproplist[i].CircName==circuitname)
//			{
//				k=i;
//				i=NumCircuits; // that will break it
//			}
//		}
//
//	// trap errors
//
//	if(k==-1)
//	{
//		CString msg="Unknown circuit";
//		lua_error(L,msg.GetBuffer(1));
//		return 0;
//	}
//
//	CComplex amps,volts,fluxlinkage;
//
//	amps =TheDoc->circproplist[k].Amps;
//	volts=TheDoc->GetVoltageDrop(k);
//	fluxlinkage=TheDoc->GetFluxLinkage(k);
//
//	lua_pushnumber(L,Re(amps));
//	lua_pushnumber(L,Im(amps));
//	lua_pushnumber(L,Re(volts));
//	lua_pushnumber(L,Im(volts));
//	lua_pushnumber(L,Re(fluxlinkage));
//	lua_pushnumber(L,Im(fluxlinkage));
//
//	return 6;
//}
//
//int FPProc_interface::gettitle(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushstring(L,thisDoc->GetTitle());
//
//	return 1;
//}
//
//
/////////////////////////////////////////////////////////
//// Functions that provide low-level access to the mesh
/////////////////////////////////////////////////////////
//
//int FPProc_interface::numnodes(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshnode.GetSize());
//
//	return 1;
//}
//
//int FPProc_interface::numelements(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	lua_pushnumber(L,thisDoc->meshelem.GetSize());
//
//	return 1;
//}
//
//int FPProc_interface::getnode(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	int k=(int) lua_todouble(L,1);
//	k--;
//	if ((k<0) || (k>=thisDoc->meshnode.GetSize())) return 0;
//
//	lua_pushnumber(L,thisDoc->meshnode[k].x);
//	lua_pushnumber(L,thisDoc->meshnode[k].y);
//
//	return 2;
//}
//
//int FPProc_interface::getelement(lua_State *L)
//{
//	//CatchNullDocument();//
//	FPProc_interface * thisDoc;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//
//	int k=(int) lua_todouble(L,1);
//	k--;
//	if ((k<0) || (k>=thisDoc->meshelem.GetSize())) return 0;
//
//	lua_pushnumber(L,thisDoc->meshelem[k].p[0]+1);
//	lua_pushnumber(L,thisDoc->meshelem[k].p[1]+1);
//	lua_pushnumber(L,thisDoc->meshelem[k].p[2]+1);
//	lua_pushnumber(L,Re(thisDoc->meshelem[k].ctr));
//	lua_pushnumber(L,Im(thisDoc->meshelem[k].ctr));
//	lua_pushnumber(L,thisDoc->ElmArea(k));
//	lua_pushnumber(L,thisDoc->blocklist[thisDoc->meshelem[k].lbl].InGroup);
//
//	return 7;
//}


/////////////////////////  JUNK //////////////////////////////

//int FPProc_interface::dumpheader(lua_State * L)
//{
//// 	CatchNullDocument();
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	return 0;
//}


// void FPProc_interface::initalise_lua()
// {
// 	// use a messagebox to display errors
// 	lua_register(lua,"mo_setfocus",lua_switchfocus);
// 	lua_register(lua,"mo_getpointvalues",lua_getpointvals);
// 	lua_register(lua,"mo_close",lua_exitpost);
// 	lua_register(lua,"mo_addcontour",lua_addcontour);
// 	lua_register(lua,"mo_clearcontour",lua_clearcontour);
// 	lua_register(lua,"mo_lineintegral",lua_lineintegral);
// 	lua_register(lua,"mo_selectblock",lua_selectblock);
// 	lua_register(lua,"mo_groupselectblock",lua_groupselectblock);
// 	lua_register(lua,"mo_clearblock",lua_clearblock);
// 	lua_register(lua,"mo_blockintegral",lua_blockintegral);
// 	lua_register(lua,"mo_zoomnatural",lua_zoomnatural);
// 	lua_register(lua,"mo_zoomin",lua_zoomin);
// 	lua_register(lua,"mo_zoomout",lua_zoomout);
// 	lua_register(lua,"mo_zoom",lua_zoom);
// 	lua_register(lua,"mo_showgrid",lua_showgrid);
// 	lua_register(lua,"mo_hidegrid",lua_hidegrid);
// 	lua_register(lua,"mo_gridsnap",lua_gridsnap);
// 	lua_register(lua,"mo_setgrid",lua_setgrid);
// 	lua_register(lua,"mo_showmesh",lua_showmesh);
// 	lua_register(lua,"mo_hidemesh",lua_hidemesh);
// 	lua_register(lua,"mo_hidedensityplot",lua_hidedensity);
// 	lua_register(lua,"mo_showdensityplot",lua_showdensity);
// 	lua_register(lua,"mo_hidecontourplot",lua_hidecountour);
// 	lua_register(lua,"mo_showcontourplot",lua_showcountour);
// 	lua_register(lua,"mo_showvectorplot",lua_vectorplot);
// 	lua_register(lua,"mo_smooth",lua_smoothing);
// 	lua_register(lua,"mo_showpoints",lua_showpoints);
// 	lua_register(lua,"mo_hidepoints",lua_hidepoints);
// 	lua_register(lua,"mo_shownames",lua_shownames);
// 	lua_register(lua,"mo_getprobleminfo",lua_getprobleminfo);
// 	lua_register(lua,"mo_savebitmap",lua_savebitmap);
// 	lua_register(lua,"mo_getcircuitproperties",lua_getcircuitprops);
// 	lua_register(lua,"mo_savemetafile",lua_saveWMF);
//     lua_register(lua,"mo_refreshview",lua_refreshview);
// 	lua_register(lua,"mo_selectpoint",lua_selectline);
// 	lua_register(lua,"mo_seteditmode",lua_seteditmode);
// 	lua_register(lua,"mo_bendcontour",lua_bendcontour);
// 	lua_register(lua,"mo_makeplot",lua_makeplot);
// 	lua_register(lua,"mo_reload",lua_reload);
// 	lua_register(lua,"mo_gradient",lua_gradient);
// 	lua_register(lua,"mo_gettitle",lua_gettitle);
// 	lua_register(lua,"mo_get_title",lua_gettitle);
//
// 	lua_register(lua,"mo_resize",luaResize);
// 	lua_register(lua,"mo_minimize",luaMinimize);
// 	lua_register(lua,"mo_maximize",luaMaximize);
// 	lua_register(lua,"mo_restore", luaRestore);
//
// 	// compatibility with alternate lua function naming convention
// 	lua_register(lua,"mo_set_focus",lua_switchfocus);
// 	lua_register(lua,"mo_get_point_values",lua_getpointvals);
// 	lua_register(lua,"mo_add_contour",lua_addcontour);
// 	lua_register(lua,"mo_clear_contour",lua_clearcontour);
// 	lua_register(lua,"mo_line_integral",lua_lineintegral);
// 	lua_register(lua,"mo_select_block",lua_selectblock);
// 	lua_register(lua,"mo_group_select_block",lua_groupselectblock);
// 	lua_register(lua,"mo_clear_block",lua_clearblock);
// 	lua_register(lua,"mo_block_integral",lua_blockintegral);
// 	lua_register(lua,"mo_zoom_natural",lua_zoomnatural);
// 	lua_register(lua,"mo_zoom_in",lua_zoomin);
// 	lua_register(lua,"mo_zoom_out",lua_zoomout);
// 	lua_register(lua,"mo_show_grid",lua_showgrid);
// 	lua_register(lua,"mo_hide_grid",lua_hidegrid);
// 	lua_register(lua,"mo_show_mesh",lua_showmesh);
// 	lua_register(lua,"mo_hide_mesh",lua_hidemesh);
// 	lua_register(lua,"mo_hide_density_plot",lua_hidedensity);
// 	lua_register(lua,"mo_show_density_plot",lua_showdensity);
// 	lua_register(lua,"mo_hide_contour_plot",lua_hidecountour);
// 	lua_register(lua,"mo_show_contour_plot",lua_showcountour);
// 	lua_register(lua,"mo_show_vector_plot",lua_vectorplot);
// 	lua_register(lua,"mo_show_points",lua_showpoints);
// 	lua_register(lua,"mo_hide_points",lua_hidepoints);
// 	lua_register(lua,"mo_grid_snap",lua_gridsnap);
// 	lua_register(lua,"mo_set_grid",lua_setgrid);
// 	lua_register(lua,"mo_get_problem_info",lua_getprobleminfo);
// 	lua_register(lua,"mo_save_bitmap",lua_savebitmap);
// 	lua_register(lua,"mo_get_circuit_properties",lua_getcircuitprops);
// 	lua_register(lua,"mo_save_metafile",lua_saveWMF);
//     lua_register(lua,"mo_refresh_view",lua_refreshview);
// 	lua_register(lua,"mo_select_point",lua_selectline);
// 	lua_register(lua,"mo_set_edit_mode",lua_seteditmode);
// 	lua_register(lua,"mo_bend_contour",lua_bendcontour);
// 	lua_register(lua,"mo_make_plot",lua_makeplot);
// 	lua_register(lua,"mo_show_names",lua_shownames);
//
// 	// functions to access low-level mesh info
// 	lua_register(lua,"mo_numnodes",lua_numnodes);
// 	lua_register(lua,"mo_numelements",lua_numelements);
// 	lua_register(lua,"mo_getnode",lua_getnode);
// 	lua_register(lua,"mo_getelement",lua_getelement);
//
// 	lua_register(lua,"mo_num_nodes",lua_numnodes);
// 	lua_register(lua,"mo_num_elements",lua_numelements);
// 	lua_register(lua,"mo_get_node",lua_getnode);
// 	lua_register(lua,"mo_get_element",lua_getelement);
//
// 	pFemmviewdoc=this;
//
// }


//int FPProc_interface::reload(lua_State * L)
//{
//	FPProc_interface * thisDoc;
//	CFemmviewView * theView;
//	thisDoc=(FPProc_interface *)pFemmviewdoc;
//	POSITION pos;
//	pos=thisDoc->GetFirstViewPosition();
//	theView=(CFemmviewView *)thisDoc->GetNextView(pos);
//
//	theView->BeginWaitCursor();
//	thisDoc->OnReload();
//	theView->EndWaitCursor();
//	if (theView->d_ResetOnReload==FALSE) thisDoc->FirstDraw=FALSE;
//	theView->RedrawView();
//	theView->LuaViewInfo();
//	return 0;
//}
