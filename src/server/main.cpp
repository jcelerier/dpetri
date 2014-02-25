#include <graphviz/gvc.h>
#include <stdio.h>

#include <QApplication>
#include <QTime>

#include "mainwindow.h"

char* toDot(char* ing, unsigned int* size)
{
  char* serializedData;
  GVC_t* gvc = gvContext();
  Agraph_t* G = agmemread(ing);

  agset(G, (char*) "label", (char*)"");
  gvLayout (gvc, G, "dot");

  gvRenderData(gvc, G, "svg", &serializedData, size);
  gvFreeLayout(gvc, G);

  agclose (G);

  gvFreeContext(gvc);
  return serializedData;
}

static char* data =
"digraph N {\n\
 graph [fontname=Helvetica nodesep=0.25 ranksep=0.25 remincross=true label=\"Petri net\\n&Omega;=((p4=0) &and; (init=0) &and; (p3=0) &and; (p9=0) &and; (p2=0) &and; (final=0) &and; (p5=0) &and; (p7=0) &and; (p1=0) &and; (p8=0) &and; (p6=0))\"]\n\
 node [fontname=Helvetica fixedsize=true width=.3 height=.3 label=\"\" style=filled]\n\
 edge [fontname=Helvetica color=white arrowhead=none weight=20.0]\n\
\n\
// interface\n\
\n\
// places\n\
 node [shape=circle fillcolor=white]\n\
p10 []\n\
 p10_l  [style=invis]\n\
 p10_l -> p10 [headlabel=final]\n\
p11 [fillcolor=black peripheries=2 height=.2 width=.2 ]\n\
 p11_l  [style=invis]\n\
 p11_l -> p11 [headlabel=init]\n\
p1  []\n\
 p1_l   [style=invis]\n\
 p1_l -> p1 [headlabel=p1]\n\
p6  []\n\
 p6_l   [style=invis]\n\
 p6_l -> p6 [headlabel=p2]\n\
p4  []\n\
 p4_l   [style=invis]\n\
 p4_l -> p4 [headlabel=p3]\n\
p7  []\n\
 p7_l   [style=invis]\n\
 p7_l -> p7 [headlabel=p4]\n\
p2  []\n\
 p2_l   [style=invis]\n\
 p2_l -> p2 [headlabel=p5]\n\
p3  []\n\
 p3_l   [style=invis]\n\
 p3_l -> p3 [headlabel=p6]\n\
p5  []\n\
 p5_l   [style=invis]\n\
 p5_l -> p5 [headlabel=p7]\n\
p9  []\n\
 p9_l   [style=invis]\n\
 p9_l -> p9 [headlabel=p8]\n\
p8  []\n\
 p8_l   [style=invis]\n\
 p8_l -> p8 [headlabel=p9]\n\
 // transitions\n\
 node [shape=box]\n\
t7  []\n\
 t7_l   [style=invis]\n\
 t7_l -> t7 [headlabel=t1]\n\
t1  []\n\
 t1_l   [style=invis]\n\
 t1_l -> t1 [headlabel=t2]\n\
t6  []\n\
 t6_l   [style=invis]\n\
 t6_l -> t6 [headlabel=t3]\n\
t8  []\n\
 t8_l   [style=invis]\n\
 t8_l -> t8 [headlabel=t4]\n\
t4  []\n\
 t4_l   [style=invis]\n\
 t4_l -> t4 [headlabel=t5]\n\
t5  []\n\
 t5_l   [style=invis]\n\
 t5_l -> t5 [headlabel=t6]\n\
t2  []\n\
 t2_l   [style=invis]\n\
 t2_l -> t2 [headlabel=t7]\n\
t3  []\n\
 t3_l   [style=invis]\n\
 t3_l -> t3 [headlabel=t8]\n\
\n\
 // inner cluster\n\
 subgraph cluster1\n\
{\n\
  t7 t7_l t1 t1_l t6 t6_l t8 t8_l t4 t4_l t5 t5_l t2 t2_l t3 t3_l\n\
  p10 p10_l p11 p11_l p1 p1_l p6 p6_l p4 p4_l p7 p7_l p2 p2_l p3 p3_l p5 p5_l p9 p9_l p8 p8_l\n\
  label=\"\" style=invis\n\
 }\n\
\n\
 // arcs\n\
 edge [fontname=Helvetica arrowhead=normal color=black]\n\
 t3 -> p10  []\n\
 p11 -> t7  []\n\
 t7 -> p1   []\n\
 p1 -> t1   []\n\
 t7 -> p6   []\n\
 p6 -> t6   []\n\
 t1 -> p4   []\n\
 p4 -> t8   []\n\
 t6 -> p7   []\n\
 p7 -> t4   []\n\
 t6 -> p2   []\n\
 p2 -> t5   []\n\
 t4 -> p3   []\n\
 t8 -> p3   []\n\
 p3 -> t2   []\n\
 p5 -> t2   []\n\
 t5 -> p5   []\n\
 p9 -> t3   []\n\
 t2 -> p9   []\n\
 t2 -> p8   []\n\
 p8 -> t3   []\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
}\n";

void submain()
{
  unsigned int sz;
  /* fputs(data, stdout); */

  char* out = toDot (data, &sz);
  FILE* f = fopen("another_test.svg", "w");

  fwrite(out, sizeof(char), sz, f);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	submain();
	MainWindow win;
	win.show();

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	return app.exec();
}


