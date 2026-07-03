//-----------------
//updated from: /home/tim/Dampe/psd/includes_lib/drawline.h
//update date: 2017.11.16
//-----------------
//2017.11.16
//	1) rename all old method with old_XXX
//	2) define new drawband and drawbandXY functions
//	3) define functions for TArrow
//
//2018.08.13
//	1) change the opt nsigma for drawband, drawbandXY and drawArrow from int to float
//-----------------
//////////////////////////////
//------new method - use TLine defined outside these draw functions, input the address as parameter
//----TLine
void setLineStyle(TLine &line, int style, int color, int width=1);
//void drawband(TLine &line, int opt, double mean, double sigma, int nsigma, double lineLow, double lineUp);
//void drawbandXY(TLine &line, double meanX, double sigmaX, int nsigmaX, double lineLowX, double lineUpX, double meanY, double sigmaY, int nsigmaY, double lineLowY, double lineUpY);
void drawband(TLine &line, int opt, double mean, double sigma, float nsigma, double lineLow, double lineUp);
void drawbandXY(TLine &line, double meanX, double sigmaX, float nsigmaX, double lineLowX, double lineUpX, double meanY, double sigmaY, float nsigmaY, double lineLowY, double lineUpY);
//----TArrow
void setArrowStyle(TArrow &arrow, int style, int lcolor, int fcolor,int width=1);
//void drawArrow(TArrow &arrow, TF1 &fitRe, double mean, double sigma, int nsigma, int style=-1, int lcolor=-1, int fcolor=-1, int width=-1);
void drawArrow(TArrow &arrow, TF1 &fitRe, double mean, double sigma, float nsigma, int style=-1, int lcolor=-1, int fcolor=-1, int width=-1);

//------old method - new each time draw line(s)
//----TLine
void old_drawline(int style, int color, double x1, double y1, double x2, double y2, int width = 2);
void old_drawbandx(int style, int color, double lower_x, double upper_x, double lower_y = 0, double upper_y = 100, int width = 2);
void old_drawbandy(int style, int color, double lower_y, double upper_y, double lower_x = 0, double upper_x = 100, int width = 2);
void old_drawpol1(int style, int color, double k, double ic, double lb=0, double ub=100, Option_t* option = "SAME", int width = 2);

//////////////////////////////
//------new method
void setLineStyle(TLine &line, int style, int color, int width)
{
	if (style!=-1) line.SetLineStyle(style);
	if (color!=-1) line.SetLineColor(color);
	if (width!=-1) line.SetLineWidth(width);
} //setLineStyle

void drawband(TLine &line, int opt, double mean, double sigma, float nsigma, double lineLow, double lineUp)
{
	if (opt==1) {
		line.DrawLine(mean - nsigma*sigma, lineLow, mean - nsigma*sigma, lineUp);
		line.DrawLine(mean + nsigma*sigma, lineLow, mean + nsigma*sigma, lineUp);
	} //X
	else if (opt==2) {
		line.DrawLine(lineLow, mean - nsigma*sigma, lineUp, mean - nsigma*sigma);
		line.DrawLine(lineLow, mean + nsigma*sigma, lineUp, mean + nsigma*sigma);
	} //Y
	else {
		cout << "Please insert correct opt. 1:X, 2:Y" << endl;
	} //wrong opt
} //drawband

void drawbandXY(TLine &line, double meanX, double sigmaX, float nsigmaX, double lineLowX, double lineUpX, double meanY, double sigmaY, float nsigmaY, double lineLowY, double lineUpY)
{
	drawband(line, 1, meanX, sigmaX, nsigmaX, lineLowX, lineUpX);
	drawband(line, 2, meanY, sigmaY, nsigmaY, lineLowY, lineUpY);
} //drawbandXY

void setArrowStyle(TArrow &arrow, int style, int lcolor, int fcolor, int width)
{
	if (style!=-1) arrow.SetLineStyle(style);
	if (lcolor!=-1) arrow.SetLineColor(lcolor);
	if (fcolor!=-1) arrow.SetFillColor(fcolor);
	if (width!=-1) arrow.SetLineWidth(width);
} //setArrowStyle

void drawArrow(TArrow &arrow, TF1 &fitRe, double mean, double sigma, float nsigma, int style, int lcolor, int fcolor, int width)
{
	setArrowStyle(arrow, style, lcolor, fcolor, width);
	double arrowY = fitRe.Eval(mean-nsigma*sigma);
	
	arrow.DrawArrow(mean-nsigma*sigma, arrowY, mean+nsigma*sigma, arrowY, 0.01, "<|>");
} //drawArrow

//------old method
void old_drawline(int style, int color, double x1, double y1, double x2, double y2, int width)
{
	TLine* line = new TLine(x1, y1, x2, y2);
	line->SetLineStyle(style);
	line->SetLineWidth(width);
	line->SetLineColor(color);
	line->Draw();
}

void old_drawbandx(int style, int color, double lower_x, double upper_x, double lower_y, double upper_y, int width)
{
	old_drawline(style, color, upper_x, lower_y, upper_x, upper_y, width);
	old_drawline(style, color, lower_x, lower_y, lower_x, upper_y, width);
}

void old_drawbandy(int style, int color, double lower_y, double upper_y, double lower_x, double upper_x, int width)
{
	old_drawline(style, color, lower_x, upper_y, upper_x, upper_y, width);
	old_drawline(style, color, lower_x, lower_y, upper_x, lower_y, width);
}

void old_drawpol1(int style, int color, double k, double ic, double lb, double ub, Option_t* option, int width)
{
	TF1* f1 = new TF1("f1", "pol1", lb, ub);
	f1->SetParameter(0, ic);
	f1->SetParameter(1, k);
	f1->SetLineStyle(style);
	f1->SetLineWidth(width);
	f1->SetLineColor(color);
	f1->Draw(option);
}
