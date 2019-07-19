#include "m_pd.h"

static t_class *buttoncombinations_class;

typedef struct _buttoncombinations {
  t_object  x_obj;
  t_int shift, capt, trig, scan;
  t_int current_count, isRecording, isShiftCapt;
  t_int decisionMade;
  t_int preventDecisionUntil;
  t_int revertShiftCapt;
  t_int shiftCaptOnShift;
  t_int shiftMode;
  t_int savingMode;
  t_int shiftScanMode;
  t_int shiftCaptScanMode;
  t_int prevScanValue;
  t_symbol *decisionName;
  t_symbol *prevDecision;
  t_inlet *in_shift, *in_capt, *in_trig, *in_scan;
  t_outlet *out;
} t_buttoncombinations;

void buttoncombinations_bang(t_buttoncombinations *x)
{
  // outlet_float(x->out, x->current_count);

  postfloat(x->decisionMade);   post(" :decisionMade");
  postfloat(x->current_count);				post(" :count");
  postfloat(x->shift);				post(" :shift");
  postfloat(x->capt);				post(" : capt");
  postfloat(x->trig);				post(" : trig");
  postfloat(x->scan);				post(" : scan");
  post("------------");
  // x->current_count++;
  // post("This will do the decisions!");
}

// void buttoncombinations_onListMsg(t_buttoncombinations *x, t_symbol *s, t_int argc, t_atom *argv){
//
//   t_symbol * curarg = atom_getsymbolarg(0, argc, argv);
//
//     switch(argc){
//             //Lists must have two arguments (to set both A and B); otherwise we'll throw an error
//         case 2:
//             if(!strcmp(curarg->s_name, "record"))
//             {
//               post("record : received");
//             }
//             break;
//         default:
//             error("[polybang ]: two arguments are needed to set a new ratio");
//     }
// }

void buttoncombinations_onRecCancelMsg(t_buttoncombinations *x){
  x->isRecording = 0;
  // x->decisionMade = 1;
  t_symbol *str = gensym("recordingOff");
    post("cancel recording");

    outlet_symbol(x->out, str);
}

void buttoncombinations_give_output(t_buttoncombinations *x)
{
  if(x->shiftMode == 0){
    outlet_symbol(x->out, gensym("shiftMode_off"));
  } else {
    outlet_symbol(x->out, gensym("shiftMode_on"));
  }
  outlet_symbol(x->out, x->decisionName);
  t_symbol *str;
  if (x->preventDecisionUntil == 1
    && x->revertShiftCapt == 1)
    {
      x->isShiftCapt = 1 - x->isShiftCapt;
      if(x->isShiftCapt == 0){
        str = gensym("shift+capt_off");
      }
      else {
        str = gensym("shift+capt_on");
      }
      x->revertShiftCapt = 0;
      outlet_symbol(x->out, str);
    }
  if (x->savingMode == 1){
    str = gensym("savingFiles");
    outlet_symbol(x->out, str);
    x->savingMode = 0;
  }
}

void buttoncombinations_check_states(t_buttoncombinations *x){
  if(x->shift == 0
    && x->capt == 0
    && x->trig == 0)
  {
    x->current_count = 0;
    x->decisionMade = 0;
    x->decisionName = gensym("none");
    x->preventDecisionUntil = 0;
    x->shiftMode = 0;
  }
  else if(x->shift == 1
    && x->capt == 0
    && x->trig == 0
    && x->preventDecisionUntil != 1)
  {
    x->shiftCaptOnShift = x->isShiftCapt;
    x->current_count = 1;
    x->decisionMade = 0;
    x->shiftMode = 1;
    x->decisionName = gensym("none");
  }
  else if(x->shift == 0
    && x->capt == 1
    && x->trig == 0
    && x->decisionMade == 0
    && x->preventDecisionUntil != 1)
  {
    if(x->isRecording == 0)
    {
      x->isRecording = 1;
      x->decisionMade = 1;
      x->decisionName = gensym("recordingOn");
    }
    else if(x->isRecording == 1)
    {
      x->isRecording = 0;
      x->decisionMade = 1;
      x->decisionName = gensym("recordingOff");
    }
    x->prevDecision = x->decisionName;
  }
  else if(x->shift == 0
    && x->capt == 0
    && x->trig == 1
    && x->decisionMade == 0
    && x->preventDecisionUntil != 1)
  {
    x->decisionName = gensym("trigger");
  }
  else if(x->shift == 1
    && x->capt >= 2
    && x->trig == 0
    && x->decisionMade == 0
    && x->preventDecisionUntil != 1)
  {
    if(x->isShiftCapt == 0)
    {
      x->decisionName = gensym("shift+capt_on");
      x->decisionMade = 1;
      x->isShiftCapt = 1;
      // x->prevDecision = x->decisionName;
    }
    else if(x->isShiftCapt == 1)
    {
      x->decisionName = gensym("shift+capt_off");
      x->decisionMade = 1;
      x->isShiftCapt = 0;
    }
  }
  else if(x->shift == 1
    && x->capt == 0
    && x->trig >= 2
    && x->decisionMade == 0
    && x->preventDecisionUntil != 1)
  {
    x->decisionName = gensym("shift+trig");
    x->decisionMade = 1;
    // x->prevDecision = x->decisionName;
  }
  else if(x->shift == 1
    && x->capt >= 2
    && x->trig >= 2
    && x->preventDecisionUntil != 1)
  {
    x->decisionName = gensym("shift+capt+trig");
    x->decisionMade = 1;
    x->preventDecisionUntil = 1;
    if (x->capt == 2 && x->trig > 2)
      x->revertShiftCapt = 1;
      x->savingMode = 1;
    // x->prevDecision = x->decisionName;
  }
  // else{
  //   x->decisionName = gensym("decision");
  // }
  buttoncombinations_give_output(x);
}

void buttoncombinations_onSet_shift(t_buttoncombinations *x, t_floatarg f){
  static t_float prevV = 0;
  if (f != prevV)
  {
    if(f == 1)
      x->shift = ++x->current_count;
    else
      x->shift = 0;
    buttoncombinations_check_states(x);
  }
  prevV = f;
}

void buttoncombinations_onSet_capt(t_buttoncombinations *x, t_floatarg f){
  static t_float prevV = 0;
  if (f != prevV)
  {
    if(f == 1)
      x->capt = ++x->current_count;
    else
      x->capt = 0;
    buttoncombinations_check_states(x);
  }
  prevV = f;
}

void buttoncombinations_onSet_trig(t_buttoncombinations *x, t_floatarg f){
  static t_float prevV = 0;
  if (f != prevV)
  {
    if(f == 1)
      x->trig = ++x->current_count;
    else
      x->trig = 0;
    buttoncombinations_check_states(x);
  }
  prevV = f;
}

void buttoncombinations_onSet_scan(t_buttoncombinations *x, t_floatarg f){
  static t_float prevV = 0;
  if (f != prevV)
  {
    if (x->shift == 1
      && x->capt == 0
      && x->trig == 0
      && x->preventDecisionUntil != 1)
    {
      x->scan = f;

      if (x->scan > x->prevScanValue + 50){
        x->prevScanValue  = x->scan;
        x->shiftScanMode = 1;
        t_symbol *str = gensym("shiftScanMode_on");
        outlet_symbol(x->out, str);
      }
      else if (x->scan < x->prevScanValue - 50){
        x->prevScanValue  = x->scan;
        x->shiftScanMode = 0;
        t_symbol *str = gensym("shiftScanMode_off");
        outlet_symbol(x->out, str);
      }
      else {

      }
    }
    else if (x->shift == 1
      && x->capt >= 2
      && x->trig == 0
      && x->preventDecisionUntil != 1)
    {
      x->scan = f;

      if (x->scan > x->prevScanValue + 50){
        x->prevScanValue  = x->scan;
        x->shiftCaptScanMode = 1;
        // post("shiftCaptScanMode_on");
        t_symbol *str = gensym("shiftCaptScanMode_on");
        outlet_symbol(x->out, str);
      }
      else if (x->scan < x->prevScanValue - 50){
        x->prevScanValue  = x->scan;
        x->shiftCaptScanMode = 0;
        // post("shiftCaptScanMode_off");
        t_symbol *str = gensym("shiftCaptScanMode_off");
        outlet_symbol(x->out, str);
      }
      else {

      }
    }
    // buttoncombinations_check_states(x);
  }
  prevV = f;
}

void *buttoncombinations_new(void)
{
  t_buttoncombinations *x = (t_buttoncombinations *)pd_new(buttoncombinations_class);

  x->in_shift = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("shift"));
  x->in_capt = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("capt"));
  x->in_trig = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("trig"));
  x->in_scan = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("scan"));

  x->out = outlet_new(&x->x_obj, &s_symbol);

  x->decisionName = gensym("none");
  x->prevDecision = x->decisionName;

  x->isRecording = 0;
  x->decisionMade = 0;
  x->preventDecisionUntil = 0;
  x->shiftMode = 0;
  x->shiftScanMode = 0;
  x->prevScanValue = 0;
  x->shiftCaptScanMode = 0;
  x->shiftCaptOnShift = 0;

  return (void *)x;
}

void buttoncombinations_free(t_buttoncombinations *x){
    inlet_free(x->in_shift);
    inlet_free(x->in_capt);
    inlet_free(x->in_trig);
    inlet_free(x->in_scan);
    outlet_free(x->out);
}

void buttoncombinations_setup(void) {
  buttoncombinations_class = class_new(gensym("buttoncombinations"),
        (t_newmethod)buttoncombinations_new,
        (t_method)buttoncombinations_free,
        sizeof(t_buttoncombinations),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        A_DEFFLOAT,
        0);
  class_addbang(buttoncombinations_class, (t_method)buttoncombinations_bang);

  //List: set the ratio and reset
  // class_addlist(buttoncombinations_class, (t_method)buttoncombinations_onListMsg);

  class_addmethod(buttoncombinations_class,
                  (t_method)buttoncombinations_onRecCancelMsg,
                  gensym("cancelRecord"),
                  0);

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

  class_addmethod(buttoncombinations_class,
                  (t_method)buttoncombinations_onSet_scan,
                  gensym("scan"),
                  A_DEFFLOAT,
                  0);
}
