#include "m_pd.h"

// #define SET_POST

static t_class *bufferlogic_class;

typedef struct _bufferlogic {
  t_object  x_obj;
  // t_int shift, capt, trig;
  t_int layer;
  t_int state[6];
  t_int noOfRecordings[6];
  t_int enableUndo[6];
  t_int current_count;
  t_inlet *in_state, *in_layer /*, *in_trig */;
  t_outlet *out_state, *out_layer;
} t_bufferlogic;

void bufferlogic_bang(t_bufferlogic *x)
{
        x->current_count++;
}

void bufferlogic_onPrintMsg(t_bufferlogic *x)
{
  // outlet_float(x->out_layer, x->layer);
  // outlet_float(x->out_state, x->state[x->layer]);
  for(int i = 0; i < 6; ++i){
    post("*****");
    postfloat(i);  post(" : ");
    postfloat(x->state[i]);				post(" layer : state");
    postfloat(x->noOfRecordings[i]);				post(" :recording");
    postfloat(x->state[i]); post(" : next bufOp");
    postfloat(x->enableUndo[i]); post(" : undo possible");
  }
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}


void bufferlogic_onRecordMsg(t_bufferlogic *x)
{
    x->enableUndo[x->layer] = 0;
    x->noOfRecordings[x->layer] += 1;
    x->state[x->layer] = 3;
    if(x->noOfRecordings[x->layer] > 1)
      x->enableUndo[x->layer] = 1;

    outlet_float(x->out_layer, x->layer);
    outlet_float(x->out_state, x->state[x->layer]);
}

void bufferlogic_onBufOpMsg(t_bufferlogic *x)
{

#ifdef SET_POST
  postfloat(x->layer); post(" on layer");
#endif
  t_int layer = x->layer;
  t_int prevState = x->state[x->layer];
  t_int noOfRec = x->noOfRecordings[x->layer];
  switch(prevState)
  {
    /*
    cases
    0 for empty buffer
    1 buffer was loaded with unto revording
    2 buffer was loaded with initial recording from USB drive
    3 buffer was recorded
    */
    case 0:
    {
#ifdef SET_POST
      postfloat(2); post(" : load from USB ");
#endif
      x->state[layer] = 2;
      x->noOfRecordings[layer] += 1;
      // x->enableUndo[x->layer] = 0;
      // buffer is empty, load file from USB drive
      break;
    }
    case 1:
    {
#ifdef SET_POST
      postfloat(0); post(" : clear ");
#endif
      x->state[layer] = 0;
      x->noOfRecordings[layer] = 0;
      x->enableUndo[x->layer] = 0;
      // buffer was undoed, so clear content now
      break;
    }
    case 2:
      // buffer was loaded with USB content, so clear content now.
    {
#ifdef SET_POST
      postfloat(0); post(" : clear ");
#endif
      x->state[layer] = 0;
      x->enableUndo[x->layer] = 0;
      break;
    }
    case 3:
      // buffer was recorded, so undo to previous buffer.
      // if it was first recording into cleared buffer, then clear buffer.
    {
      if(noOfRec > 1)
      {
#ifdef SET_POST
        postfloat(1); post(" : undo ");
#endif
        x->state[layer] = 1;
        x->enableUndo[x->layer] = 0;
      }
      else{
#ifdef SET_POST
        postfloat(0); post(" : clear ");
#endif
        // x->noOfRecordings[layer] = 0;
        x->enableUndo[x->layer] = 0;
        x->state[layer] = 0;
      }
      break;
    }
    default:
      break;
  }
#ifdef SET_POST
  postfloat(x->state[x->layer]); post(" : do bufOp");
  postfloat(x->enableUndo[x->layer]); post(" : undo possible");
  post("------------");
#endif

  outlet_float(x->out_layer, x->layer);
  outlet_float(x->out_state, x->state[x->layer]);
}

void bufferlogic_check_states(t_bufferlogic *x){
  postfloat(x->noOfRecordings[x->layer]);				post(" :recording");
  postfloat(x->state[x->layer]);				post(" :state");
  post("------------");
}

void bufferlogic_onSet_state(t_bufferlogic *x, t_floatarg f){
t_int state = f;
  if (f<0)
    state = 0;
  else if (f > 3)
    {
      // state = 0;
      post("WARNING: undefined state. Value has been ignored.");
    }
  else
    x->state[x->layer] = state;
  // bufferlogic_check_states(x);
}

void bufferlogic_onSet_layer(t_bufferlogic *x, t_floatarg f){
t_int layer;
  if (f<0)
    layer = 0;
  else if (f > 5)
    layer = 5;
  else
    layer = (t_int)f;
  x->layer = layer;
}

// void bufferlogic_onSet_trig(t_bufferlogic *x, t_floatarg f){
//   if(f == 1)
//     x->trig = ++x->current_count;
//   else
//     x->trig = 0;
//   bufferlogic_check_states(x);
// }

void *bufferlogic_new(void)
{
  t_bufferlogic *x = (t_bufferlogic *)pd_new(bufferlogic_class);

  x->in_state = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("currentState"));
  x->in_layer = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("currentLayer"));
  // x->in_trig = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("trig"));

  x->out_state = outlet_new(&x->x_obj, &s_float);
  x->out_layer = outlet_new(&x->x_obj, &s_float);

  x->layer = 0;
  for (int i = 0; i < 6; ++i)
  {
    x->state[i] = 0;
    x->noOfRecordings[i] = 0;
  }
  return (void *)x;
}

void bufferlogic_free(t_bufferlogic *x){
    inlet_free(x->in_state);
    inlet_free(x->in_layer);
    outlet_free(x->out_state);
    outlet_free(x->out_layer);
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
                  (t_method)bufferlogic_onRecordMsg,
                  gensym("logRecord"),
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onPrintMsg,
                  gensym("print"),
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onBufOpMsg,
                  gensym("getBufOp"),
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onSet_state,
                  gensym("currentState"),
                  A_DEFFLOAT,
                  0);

  class_addmethod(bufferlogic_class,
                  (t_method)bufferlogic_onSet_layer,
                  gensym("currentLayer"),
                  A_DEFFLOAT,
                  0);

  // class_addmethod(bufferlogic_class,
  //                 (t_method)bufferlogic_onSet_trig,
  //                 gensym("trig"),
  //                 A_DEFFLOAT,
  //                 0);
}
