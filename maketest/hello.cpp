// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QApplication>
#include <QWidget>
#include "hello.h"

int main (int argc, char *argv[])
{
  QApplication a(argc, argv);
  QWidget *w = new QWidget();
  w->show();
  printf("hello to the world, %s - Version %d.%d\n", argv[0], Hello_VERSION_MAJOR, Hello_VERSION_MINOR);
  a.exec();


  int x = 4;
  //*(int*)nullptr = 100; // test breaks and stuff
  if (argc < 2)
    {
    fprintf(stdout,"Usage: %s number\n",argv[0]);
    return 1;
    }
  //printf("hello to the world, %s - Version %d.%d\n", argv[0], Hello_VERSION_MAJOR, Hello_VERSION_MINOR);
  double inputValue = atof(argv[1]);
  double outputValue = sqrt(inputValue);
  fprintf(stdout,"The square root of %g is %g\n",
          inputValue, outputValue);
  return 0;
}