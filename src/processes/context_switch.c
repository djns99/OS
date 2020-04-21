#include "context_switch.h"

//context_t backup_current_context();
//void restore_context( context_t );

//void context_switch( context_t new_context, context_t* old_context_out )
//{
//    *old_context_out = backup_current_context();
//    restore_context( new_context );
//}
//
//void context_switch( context_t new_context, context_t* old_context_out, interrupt_params_t* interrupt_state )
//{
//    *old_context_out = backup_current_context();
//    restore_context( new_context );
//}