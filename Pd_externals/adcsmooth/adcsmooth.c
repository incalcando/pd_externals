#include "m_pd.h"
#include <math.h>

static t_class *adcsmooth_class;

typedef struct _adcsmooth {
  t_object  x_obj;
  t_int nVal, minDif, cVal, lastVal, cDiv;
  t_int minRiseCount, minFallCount, ignoreWithin;
  t_int noToBeConsidered;
  t_int prevValues[100];
  t_int prevDiv[100];
  t_int current_count;
  t_int riseCount, fallCount;
  t_inlet *in_nVal, *in_minDif;
  t_outlet *out, *out_fCount, *out_rCount;
} t_adcsmooth;

void adcsmooth_bang(t_adcsmooth *x)
{
    outlet_float(x->out, x->cVal);
  // post("This will do the decisions!");
}

t_float adcsmooth_calcDiviation(t_adcsmooth *x){

  int n = x->noToBeConsidered;
  float sum = 0.0, mean, standardDeviation = 0.0;
    int i;
    for(i=0; i<n; ++i)
    {
        sum += x->prevValues[i];
    }
    mean = sum/n;
    for(i=0; i<n; ++i)
        standardDeviation += pow(x->prevValues[i] - mean, 2);

    return sqrt(standardDeviation/n);

}

t_float adcsmooth_calcWeightDiv(t_adcsmooth *x){
  float sum = 0.0, mean, standardDeviation = 0.0;
    int i;
    int p = x->current_count;
    int c = 0;
    int n = x->noToBeConsidered;
    float factor = 1 / (float)n * 0.5;
    for(i=0; i<n; ++i)
    {
        sum += x->prevValues[i];
    }
    mean = sum/n;
    // postfloat(factor); post(" :factor");

    for (i = p; i<n; ++i)
    {
      // postfloat(i); post(" :i");
      // postfloat(c); post(" :c");
      // if(p == i){
      //   postfloat(x->prevValues[i]); post(" :value");
      //   postfloat(c); post(" :c");
      // }
      standardDeviation += pow(((x->prevValues[i] - mean)*(1-(c*factor))), 2);
      c++;

    }
    for (i = 0; i < p; ++i)
    {
      // postfloat(i); post(" :i");
      // postfloat(c); post(" :c");
      standardDeviation += pow(((x->prevValues[i] - mean)*(1-(c*factor))), 2);
      c++;

    }
    // post("-------");
    return sqrt(standardDeviation/n);
}

void adcsmooth_onSet_newValue(t_adcsmooth *x, t_floatarg f){
  t_int n = (t_int)f;
  t_int c = x->cVal;
  t_int d = x->minDif;
  // t_int l = x->lastVal;
  // t_int cDiv = n - l;

  // x->prevDiv[x->current_count] = cDiv;
  x->prevValues[x->current_count] = f;


  if(n > c + x->ignoreWithin){
    x->riseCount++;
    x->fallCount--;
  }else if(n < c - x->ignoreWithin){
    x->riseCount--;
    x->fallCount++;
  }
  else{
    x->riseCount--;
    x->fallCount--;
  }

  if(x->riseCount < 0)
    x->riseCount = 0;
  if(x->fallCount < 0)
    x->fallCount = 0;

  // outlet_float(x->out_rCount, x->riseCount);
  // outlet_float(x->out_fCount, x->fallCount);

  outlet_float(x->out_rCount, adcsmooth_calcDiviation(x));
  outlet_float(x->out_fCount, adcsmooth_calcWeightDiv(x));

    if(n >= c + d || n <= c - d ){
      x->cVal = n;
      outlet_float(x->out, x->cVal);
    }
    else{
      if(x->riseCount > x->minRiseCount || x->fallCount > x->minFallCount){
        x->cVal = n;
        // x->riseCount = 0;
        // x->fallCount = 0;
        outlet_float(x->out, x->cVal);
      }
    }
    x->current_count++;
    if(x->current_count > x->noToBeConsidered)
      x->current_count = 0;
}

void adcsmooth_onSet_minDif(t_adcsmooth *x, t_floatarg f){
  x->minDif = (t_int)f;
}

void adcsmooth_setArgs(t_adcsmooth *x,
                        t_floatarg f1,
                        t_floatarg f2,
                        t_floatarg f3,
                        t_floatarg f4,
                        t_floatarg f5)
{
  if(f1 > 0)
    x->noToBeConsidered = (t_int)f1;
  else
    x->noToBeConsidered = 10;

    x->minDif = (t_int)f2;
    if(f3 <= 0)
      x->minRiseCount = 5;
    else
      x->minRiseCount = (t_int)f3;

    if(f4 <= 0)
      x->minFallCount = 5;
    else
      x->minFallCount = (t_int)f4;

      x->ignoreWithin = (t_int)f5;
}

void *adcsmooth_new(t_floatarg f1, t_floatarg f2, t_floatarg f3, t_floatarg f4, t_float f5)
{
  t_adcsmooth *x = (t_adcsmooth *)pd_new(adcsmooth_class);

  x->in_nVal = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("newValue"));
  x->in_minDif = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("minDif"));

  x->out = outlet_new(&x->x_obj, &s_float);
  x->out_fCount = outlet_new(&x->x_obj, &s_float);
  x->out_rCount = outlet_new(&x->x_obj, &s_float);

  x->current_count = 0;

  x->noToBeConsidered = 10;

  // x->cVal = 0;
  // x->minDif = 0;
  adcsmooth_setArgs(x, f1, f2, f3, f4, f5);
  x->riseCount = 0;
  x->fallCount = 0;

  return (void *)x;
}

void adcsmooth_free(t_adcsmooth *x){
    inlet_free(x->in_nVal);
    inlet_free(x->in_minDif);
    outlet_free(x->out);
    outlet_free(x->out_fCount);
    outlet_free(x->out_rCount);
}

void adcsmooth_setup(void) {
  adcsmooth_class = class_new(gensym("adcsmooth"),
        (t_newmethod)adcsmooth_new,
        (t_method)adcsmooth_free,
        sizeof(t_adcsmooth),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        0);
  class_addbang(adcsmooth_class, (t_method)adcsmooth_bang);

  class_addmethod(adcsmooth_class,
                  (t_method)adcsmooth_onSet_newValue,
                  gensym("newValue"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(adcsmooth_class,
                  (t_method)adcsmooth_onSet_minDif,
                  gensym("minDif"),
                  A_DEFFLOAT,
                  0);
}
