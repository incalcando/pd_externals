#include "m_pd.h"

static t_class *bufferlogic_class;

typedef struct _bufferlogic {
  t_object  x_obj;
  t_int shift, capt, trig;
  t_int current_count;
  t_inlet *in_shift, *in_capt, *in_trig;
  t_outlet *out;
} t_bufferlogic;

void bufferlogic_bang(t_bufferlogic *x)
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

void bufferlogic_check_states(t_bufferlogic *x){
  if(x->shift == 0 && x->capt == 0 && x->trig == 0)
  {
    x->current_count = 0;
  }
}

void bufferlogic_onSet_shift(t_bufferlogic *x, t_floatarg f){
  if(f == 1)
    x->shift = ++x->current_count;
  else
    x->shift = 0;
  bufferlogic_check_states(x);
}

void bufferlogic_onSet_capt(t_bufferlogic *x, t_floatarg f){
  if(f == 1)
    x->capt = ++x->current_count;
  else
    x->capt = 0;
  bufferlogic_check_states(x);
}

void bufferlogic_onSet_trig(t_bufferlogic *x, t_floatarg f){
  if(f == 1)
    x->trig = ++x->current_count;
  else
    x->trig = 0;
  bufferlogic_check_states(x);
}

void *bufferlogic_new(void)
{
  t_bufferlogic *x = (t_bufferlogic *)pd_new(bufferlogic_class);

  x->in_shift = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("shift"));
  x->in_capt = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("capt"));
  x->in_trig = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("trig"));

  x->out = outlet_new(&x->x_obj, &s_float);


  return (void *)x;
}

void bufferlogic_free(t_bufferlogic *x){
    inlet_free(x->in_shift);
    inlet_free(x->in_capt);
    inlet_free(x->in_trig);
    outlet_free(x->out);
}

void bufferlogic_setup(void) {
  bufferlogic_class = class_new(gensym("bufferlogic"),
        (t_newmethod)bufferlogic_new,
        (t_method)bufferlogic_free,
        sizeof(t_bufferlogic),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        0);
  class_addbang(bufferlogic_class, (t_method)bufferlogic_bang);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onSet_shift,
                  gensym("shift"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onSet_capt,
                  gensym("capt"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onSet_trig,
                  gensym("trig"),
                  A_DEFFLOAT,
                  0);
}
