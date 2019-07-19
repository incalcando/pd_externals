#include "m_pd.h"

static t_class *layerlogic_class;

typedef struct _layerlogic {
  t_object  x_obj;
  t_int playLayer, recLayer, omega;
  t_int current_count;
  t_int pleds[7];
  t_int rleds[7];
  t_int leds[7];
  t_int sharedPlayAndRec;
  t_inlet *in_play, *in_rec, *in_layer;
  t_outlet *out1;
  t_outlet *out2;
  t_outlet *out3;
  t_outlet *out4;
  t_outlet *out5;
  t_outlet *out6;
  t_outlet *out7;

} t_layerlogic;

void layerlogic_bang(t_layerlogic *x)
{

  postfloat(x->playLayer);				post(" :playLayer");
  postfloat(x->recLayer);				post(" :recLayer");
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

void layerlogic_give_output(t_layerlogic *x)
{
  outlet_float(x->out1, x->leds[0]);
  outlet_float(x->out2, x->leds[1]);
  outlet_float(x->out3, x->leds[2]);
  outlet_float(x->out4, x->leds[3]);
  outlet_float(x->out5, x->leds[4]);
  outlet_float(x->out6, x->leds[5]);
  outlet_float(x->out7, x->leds[6]);
}

void layerlogic_check_states(t_layerlogic *x){
  // x->led1 = x->playLayer;
  // x->led2 = x->recLayer;
  for (int i=0; i < 6; ++i)
  {
    if(i+1 == x->playLayer)
    {
      x->pleds[i] = 2;
      if(x->recLayer == 7)
        x->pleds[i] += 1;
    }
    else
    {
      x->pleds[i] = 0;
    }
    if(i+1 == x->recLayer && x->recLayer != 7)
    {
      x->rleds[i] = 1;
    }
    else
    {
      x->rleds[i] = 0;
    }
    x->leds[i] = x->pleds[i]+x->rleds[i];
  }
  x->leds[6] = x->omega;
  layerlogic_give_output(x);
}

void layerlogic_onSet_play(t_layerlogic *x, t_floatarg f){
  x->playLayer = f;
  layerlogic_check_states(x);
}

void layerlogic_onSet_rec(t_layerlogic *x, t_floatarg f){
  x->recLayer = f;
  layerlogic_check_states(x);
}

void layerlogic_onSet_layer(t_layerlogic *x, t_floatarg f){
  if (f > 6)
    x->omega = 1;
  else
    x->omega = 0;
  layerlogic_check_states(x);
}

void *layerlogic_new(void)
{
  t_layerlogic *x = (t_layerlogic *)pd_new(layerlogic_class);

  x->in_play = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("playLayer"));
  x->in_rec = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("recLayer"));
  x->in_layer = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("getLayer"));

  x->out1 = outlet_new(&x->x_obj, &s_float);
  x->out2 = outlet_new(&x->x_obj, &s_float);
  x->out3 = outlet_new(&x->x_obj, &s_float);
  x->out4 = outlet_new(&x->x_obj, &s_float);
  x->out5 = outlet_new(&x->x_obj, &s_float);
  x->out6 = outlet_new(&x->x_obj, &s_float);
  x->out7 = outlet_new(&x->x_obj, &s_float);

  x->playLayer = 1;
  x->recLayer = 1;
  x->omega = 0;

  x->sharedPlayAndRec = 1;

  return (void *)x;
}

void layerlogic_free(t_layerlogic *x){
    inlet_free(x->in_play);
    inlet_free(x->in_rec);
    inlet_free(x->in_layer);
    outlet_free(x->out1);
    outlet_free(x->out2);
    outlet_free(x->out3);
    outlet_free(x->out4);
    outlet_free(x->out5);
    outlet_free(x->out6);
    outlet_free(x->out7);
}

void layerlogic_setup(void) {
  layerlogic_class = class_new(gensym("layerlogic"),
        (t_newmethod)layerlogic_new,
        (t_method)layerlogic_free,
        sizeof(t_layerlogic),
        CLASS_DEFAULT,
        0);
  class_addbang(layerlogic_class, (t_method)layerlogic_bang);

  class_addmethod(layerlogic_class,
                  (t_method)layerlogic_onSet_play,
                  gensym("playLayer"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(layerlogic_class,
                  (t_method)layerlogic_onSet_rec,
                  gensym("recLayer"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(layerlogic_class,
                  (t_method)layerlogic_onSet_layer,
                  gensym("getLayer"),
                  A_DEFFLOAT,
                  0);

}
