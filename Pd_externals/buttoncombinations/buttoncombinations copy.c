#include "m_pd.h"

static t_class *buttoncombinations_class;

typedef struct _buttoncombinations {
  t_object  x_obj;
  t_int shift, capt, trig;
  t_int current_count;
  t_inlet *in_shift, *in_capt, *in_trig;
  t_outlet *out;
} t_buttoncombinations;

void buttoncombinations_bang(t_buttoncombinations *x)
{
  outlet_float(x->out, x->current_count);
  postfloat(x->current_count);				post(" :count");
  postfloat(x->shift);				post(" :shift");
  postfloat(x->capt);				post(" :capt");
  postfloat(x->trig);				post(" :trig");
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

void buttoncombinations_check_states(t_buttoncombinations *x){
  if(x->shift == 0 && x->capt == 0 && x->trig == 0)
  {
    x->current_count = 0;
  }
}

void buttoncombinations_onSet_shift(t_buttoncombinations *x, t_floatarg f){
  if(f == 1)
    x->shift = ++x->current_count;
  else
    x->shift = 0;
  buttoncombinations_check_states(x);
}

void buttoncombinations_onSet_capt(t_buttoncombinations *x, t_floatarg f){
  if(f == 1)
    x->capt = ++x->current_count;
  else
    x->capt = 0;
  buttoncombinations_check_states(x);
}

void buttoncombinations_onSet_trig(t_buttoncombinations *x, t_floatarg f){
  if(f == 1)
    x->trig = ++x->current_count;
  else
    x->trig = 0;
  buttoncombinations_check_states(x);
}

void *buttoncombinations_new(void)
{
  t_buttoncombinations *x = (t_buttoncombinations *)pd_new(buttoncombinations_class);

  x->in_shift = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("shift"));
  x->in_capt = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("capt"));
  x->in_trig = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("trig"));

  x->out = outlet_new(&x->x_obj, &s_float);


  return (void *)x;
}

void buttoncombinations_free(t_buttoncombinations *x){
    inlet_free(x->in_shift);
    inlet_free(x->in_capt);
    inlet_free(x->in_trig);
    outlet_free(x->out);
}

void buttoncombinations_setup(void) {
  buttoncombinations_class = class_new(gensym("buttoncombinations"),
        (t_newmethod)buttoncombinations_new,
        (t_method)buttoncombinations_free,
        sizeof(t_buttoncombinations),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        0);
  class_addbang(buttoncombinations_class, (t_method)buttoncombinations_bang);

  class_addmethod(buttoncombinations_class,
                  (t_method)buttoncombinations_onSet_shift,
                  gensym("shift"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(buttoncombinations_class,
                  (t_method)buttoncombinations_onSet_capt,
                  gensym("capt"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(buttoncombinations_class,
                  (t_method)buttoncombinations_onSet_trig,
                  gensym("trig"),
                  A_DEFFLOAT,
                  0);
}
