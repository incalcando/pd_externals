#include "m_pd.h"

static t_class *adcsmooth_class;

typedef struct _adcsmooth {
  t_object  x_obj;
  t_int nVal, minDif, cVal, cDiv, minRiseCount, minFallCount, ignoreWithin;
  t_int prevValues[5];
  t_int prevDiv[5];
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

void adcsmooth_onSet_newValue(t_adcsmooth *x, t_floatarg f){
  t_int n = (t_int)f;
  t_int c = x->cVal;
  t_int d = x->minDif;

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

  outlet_float(x->out_rCount, x->riseCount);
  outlet_float(x->out_fCount, x->fallCount);

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
    x->cVal = (t_int)f1;
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
