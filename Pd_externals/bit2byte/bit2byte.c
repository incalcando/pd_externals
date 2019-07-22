#include "m_pd.h"

static t_class *bit2byte_class;

typedef struct _bit2byte {
  t_object  x_obj;
  t_int outValue;
  t_int bit7, bit6, bit5, bit4, bit3, bit2, bit1;
  t_int current_count;
  t_int ledByteValues[7];
  t_inlet *in_bit7, *in_bit6, *in_bit5, *in_bit4, *in_bit3, *in_bit2, *in_bit1, *in_b2b;
  t_outlet *out;
} t_bit2byte;

void bit2byte_bang(t_bit2byte *x)
{
  outlet_float(x->out, x->outValue);
  postfloat(x->outValue);				post(" :outValue");
  // postfloat(x->bit6);				post(" :6");
  // postfloat(x->bit5);				post(" :5");
  // postfloat(x->bit4);				post(" :4");
  // postfloat(x->bit3);				post(" :3");
  // postfloat(x->bit2);				post(" :2");
  // postfloat(x->bit1);				post(" :1");
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

void bit2byte_give_output(t_bit2byte *x)
{
  outlet_float(x->out, x->outValue);
}

void bit2byte_check_states(t_bit2byte *x){

}

void bit2byte_onSet_bit7(t_bit2byte *x, t_floatarg f){
  // 01000000
  if(f >= 1)
    x->outValue |= (1 << 6);
  else
    x->outValue &= ~(1 << 6);
    bit2byte_give_output(x);
}

void bit2byte_onSet_bit6(t_bit2byte *x, t_floatarg f){
  if(f >= 1)
    x->outValue |= (1 << 5);
  else
    x->outValue &= ~(1 << 5);
  bit2byte_give_output(x);
}

void bit2byte_onSet_bit5(t_bit2byte *x, t_floatarg f){
  if(f >= 1)
    x->outValue |= (1 << 4);
  else
    x->outValue &= ~(1 << 4);
  bit2byte_give_output(x);
}

void bit2byte_onSet_bit4(t_bit2byte *x, t_floatarg f){
  if(f >= 1)
    x->outValue |= (1 << 3);
  else
    x->outValue &= ~(1 << 3);
  bit2byte_give_output(x);
}

void bit2byte_onSet_bit3(t_bit2byte *x, t_floatarg f){
  if(f >= 1)
    x->outValue |= (1 << 2);
  else
    x->outValue &= ~(1 << 2);
  bit2byte_give_output(x);
}

void bit2byte_onSet_bit2(t_bit2byte *x, t_floatarg f){
  if(f >= 1)
    x->outValue |= (1 << 1);
  else
    x->outValue &= ~(1 << 1);
  bit2byte_give_output(x);
}

void bit2byte_onSet_bit1(t_bit2byte *x, t_floatarg f){
  if(f >= 1){
    x->outValue |= 1;
    x->bit1 = 1;
  }
  else {
    x->outValue &= ~1;
    x->bit1 = 0;
  }
  bit2byte_give_output(x);
}

void bit2byte_onSet_b2b(t_bit2byte *x, t_floatarg f){
  if(f >= 1 && f < 7){
    x->outValue = x->ledByteValues[(int)(f-1)];
    x->outValue |= x->bit1;
    bit2byte_give_output(x);

  }
}

void *bit2byte_new(void)
{
  t_bit2byte *x = (t_bit2byte *)pd_new(bit2byte_class);

  x->in_bit7 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit7"));
  x->in_bit6 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit6"));
  x->in_bit5 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit5"));
  x->in_bit4 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit4"));
  x->in_bit3 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit3"));
  x->in_bit2 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit2"));
  x->in_bit1 = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("bit1"));
  x->in_b2b = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("b2b"));

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

void bit2byte_free(t_bit2byte *x){
    inlet_free(x->in_bit7);
    inlet_free(x->in_bit6);
    inlet_free(x->in_bit5);
    inlet_free(x->in_bit4);
    inlet_free(x->in_bit3);
    inlet_free(x->in_bit2);
    inlet_free(x->in_bit1);
    inlet_free(x->in_b2b);
    outlet_free(x->out);
}

void bit2byte_setup(void) {
  bit2byte_class = class_new(gensym("bit2byte"),
        (t_newmethod)bit2byte_new,
        (t_method)bit2byte_free,
        sizeof(t_bit2byte),
        CLASS_DEFAULT,
        0);
  class_addbang(bit2byte_class, (t_method)bit2byte_bang);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit7,
                  gensym("bit7"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit6,
                  gensym("bit6"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit5,
                  gensym("bit5"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit4,
                  gensym("bit4"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit3,
                  gensym("bit3"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit2,
                  gensym("bit2"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_bit1,
                  gensym("bit1"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bit2byte_class,
                  (t_method)bit2byte_onSet_b2b,
                  gensym("b2b"),
                  A_DEFFLOAT,
                  0);
}
