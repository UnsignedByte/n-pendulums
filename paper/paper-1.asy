if(!settings.multipleView) settings.batchView=false;
settings.tex="pdflatex";
defaultfilename="paper-1";
if(settings.render < 0) settings.render=4;
settings.outformat="";
settings.inlineimage=true;
settings.embed=true;
settings.toolbar=false;
viewportmargin=(2,2);

import astylib;

pair R,A,B,AY,BY;
real TA,TB,L;
L = 100; // length
astyscale = 15;

R = (0,0);
TA = 45;
A = R+dir(270+TA)*L;
AY = (R.x, A.y);
TB = -30;
B = A+dir(270+TB)*L;
BY = (A.x, B.y);

pen guide = linetype(new real[] {0,5});

draw(R--A--B);
draw(astyangle(AY, R, A));
draw(astyangle(B, A, BY));
draw(astycomponents(R, A), guide);
draw(astycomponents(A, B), guide);

label("$R$", R, -dir(270+TA));
label("$A$", A, E);
label("$B$", B, dir(270+TB));
label("$l$", R--A, dir(TA));
label("$l$", A--B, -dir(TB));
label("$y_1$", (R.x, A.y)--R, W);
label("$x_1$", (R.x, A.y)--A, S);
label("$y_2$", (A.x, B.y)--A, E);
label("$x_2$", (A.x, B.y)--B, S);
label("$\theta_1$", R+dir(270+TA/2)*astyscale, dir(270+TA/2));
label("$\theta_2$", A+dir(270+TB/2)*astyscale, dir(270+TB/2));
