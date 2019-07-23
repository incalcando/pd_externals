#include "m_pd.h"

static t_class *layerlogic_new_class;

typedef struct _layerlogic_new {
  t_object  x_obj;
  t_int playLayer, recLayer, omega;
  // t_int current_count;
  t_int ledByteValues[7];
  t_int sharedPlayAndRec;
  t_inlet *in_play, *in_rec, *in_layer;
  t_outlet *out_playLeds;
  t_outlet *out_recLeds;


} t_layerlogic_new;

void layerlogic_new_bang(t_layerlogic_new *x)
{

  postfloat(x->playLayer);				post(" :playLayer");
  postfloat(x->recLayer);				post(" :recLayer");
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

void layerlogic_new_give_output(t_layerlogic_new *x)
{
  outlet_float(x->out_playLeds, x->ledByteValues[x->playLayer - 1] + x->omega);
  if(x->sharedPlayAndRec == 1)
  {
    outlet_float(x->out_recLeds, x->ledByteValues[x->playLayer - 1]);
  }
  else
    outlet_float(x->out_recLeds, x->ledByteValues[x->recLayer - 1]);

}

// t_float layerlogic_new_getLedByte(t_layerlogic_new *x, t_float layer, t_float omega){
//
//   t_float byteValue = x->ledByteValues[layer] + omega;
//
//   return byteValue;
// }

void layerlogic_new_onSet_play(t_layerlogic_new *x, t_floatarg f){
  x->playLayer = f;
  layerlogic_new_give_output(x);
}

void layerlogic_new_onSet_rec(t_layerlogic_new *x, t_floatarg f){
  if (f < 7){
    x->recLayer = f;
    x->sharedPlayAndRec = 0;
  }
  else
    x->sharedPlayAndRec = 1;
  layerlogic_new_give_output(x);
}

void layerlogic_new_onSet_layer(t_layerlogic_new *x, t_floatarg f){
  if (f > 6)
    x->omega = 1;
  else
    x->omega = 0;
  layerlogic_new_give_output(x);
}

void *layerlogic_new_new(void)
{
  t_layerlogic_new *x = (t_layerlogic_new *)pd_new(layerlogic_new_class);

  x->in_play = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("playLayer"));
  x->in_rec = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("recLayer"));
  x->in_layer = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("getLayer"));

  x->out_playLeds = outlet_new(&x->x_obj, &s_float);
  x->out_recLeds = outlet_new(&x->x_obj, &s_float);


  x->playLayer = 1;
  x->recLayer = 1;
  x->omega = 0;

  x->ledByteValues[0] = 64;
  x->ledByteValues[1] = 32;
  x->ledByteValues[2] = 16;
  x->ledByteValues[3] = 8;
  x->ledByteValues[4] = 4;
  x->ledByteValues[5] = 2;

  x->sharedPlayAndRec = 0;

  return (void *)x;
}

void layerlogic_new_free(t_layerlogic_new *x){
    inlet_free(x->in_play);
    inlet_free(x->in_rec);
    inlet_free(x->in_layer);
    outlet_free(x->out_playLeds);
    outlet_free(x->out_recLeds);
}

void layerlogic_new_setup(void) {
  layerlogic_new_class = class_new(gensym("layerlogic_new"),
        (t_newmethod)layerlogic_new_new,
        (t_method)layerlogic_new_free,
        sizeof(t_layerlogic_new),
        CLASS_DEFAULT,
        0);
  class_addbang(layerlogic_new_class, (t_method)layerlogic_new_bang);

  class_addmethod(layerlogic_new_class,
                  (t_method)layerlogic_new_onSet_play,
                  gensym("playLayer"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(layerlogic_new_class,
                  (t_method)layerlogic_new_onSet_rec,
                  gensym("recLayer"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(layerlogic_new_class,
                  (t_method)layerlogic_new_onSet_layer,
                  gensym("getLayer"),
                  A_DEFFLOAT,
                  0);

}
