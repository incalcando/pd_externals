#include "m_pd.h"

static t_class *layer_leds_class;

typedef struct _layer_leds {
  t_object  x_obj;
  t_int outValue;
  t_int bit1;
  t_int current_count;
  t_int ledByteValues[7];
  t_inlet *in_bit1, *in_b2b;
  t_outlet *out;
} t_layer_leds;

void layer_leds_bang(t_layer_leds *x)
{
  outlet_float(x->out, x->outValue);
  // postfloat(x->outValue);				post(" :outValue");

  // post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

void layer_leds_give_output(t_layer_leds *x)
{
  outlet_float(x->out, x->outValue);
}

void layer_leds_check_states(t_layer_leds *x){

}

void layer_leds_onSet_bit1(t_layer_leds *x, t_floatarg f){
  if(f >= 1){
    x->outValue |= 1;
    x->bit1 = 1;
  }
  else {
    x->outValue &= ~1;
    x->bit1 = 0;
  }
  layer_leds_give_output(x);
}

void layer_leds_onSet_b2b(t_layer_leds *x, t_floatarg f){
  if(f >= 1 && f < 7){
    x->outValue = x->ledByteValues[(int)(f-1)];
    x->outValue |= x->bit1;
    layer_leds_give_output(x);

  }
}

void *layer_leds_new(void)
{
  t_layer_leds *x = (t_layer_leds *)pd_new(layer_leds_class);

  x->in_b2b = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("b2b"));
  x->in_bit1 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit1"));


  x->out = outlet_new(&x->x_obj, &s_float);

//  = {64, 32, 16, 8, 4, 2, 1};
  x->ledByteValues[0] = 64;
  x->ledByteValues[1] = 32;
  x->ledByteValues[2] = 16;
  x->ledByteValues[3] = 8;
  x->ledByteValues[4] = 4;
  x->ledByteValues[5] = 2;

  return (void *)x;
}

void layer_leds_free(t_layer_leds *x){
    inlet_free(x->in_bit1);
    inlet_free(x->in_b2b);
    outlet_free(x->out);
}

void layer_leds_setup(void) {
  layer_leds_class = class_new(gensym("layer_leds"),
        (t_newmethod)layer_leds_new,
        (t_method)layer_leds_free,
        sizeof(t_layer_leds),
        CLASS_DEFAULT,
        0);
  class_addbang(layer_leds_class, (t_method)layer_leds_bang);


  class_addmethod(layer_leds_class,
                  (t_method)layer_leds_onSet_bit1,
                  gensym("bit1"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(layer_leds_class,
                  (t_method)layer_leds_onSet_b2b,
                  gensym("b2b"),
                  A_DEFFLOAT,
                  0);
}
