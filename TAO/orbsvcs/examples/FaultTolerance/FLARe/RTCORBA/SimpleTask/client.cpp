// $Id$

#include <sstream>
#include "WorkerC.h"
#include "ace/Get_Opt.h"
#include "tao/RTCORBA/RTCORBA.h"
#include "orbsvcs/orbsvcs/LWFT/LWFT_Client_Init.h"
#include "Client_Timer_Handler.h"
#include "ace/Sig_Handler.h"
#include "ace/Reactor.h"

const ACE_TCHAR *ior1 = ACE_TEXT("file://test.ior");
long iterations = 0;
ACE_Time_Value period;
double execution_time = 100;
std::string server_id = "server";
bool logging = false;
bool kill_me = false;

int
parse_args (int argc, ACE_TCHAR *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, ACE_TEXT("p:r:e:s:i:lk"));
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'p':
        ior1 = get_opts.opt_arg ();
        break;
      case 'r':
	period.msec (atoi (get_opts.opt_arg ()));
	break;
      case 'e':
	execution_time = atof (get_opts.opt_arg ());
	break;
      case 's':
	server_id = get_opts.opt_arg ();
	break;
      case 'i':
        iterations = atoi (get_opts.opt_arg ());
	break;
      case 'l':
	logging = true;
	break;
      case 'k':
	kill_me = true;
	break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-p <ior> "
                           "-o <ior> "
                           "\n",
                           argv [0]),
                          -1);
      }

  return 0;
}

int
check_for_nil (CORBA::Object_ptr obj, const char *msg)
{
  if (CORBA::is_nil (obj))
    ACE_ERROR_RETURN ((LM_ERROR,
                       "ERROR: Object reference <%C> is nil\n",
                       msg),
                      -1);
  else
    return 0;
}

CORBA::Short
check_policy (DeCoRAM::Worker_ptr server)
{
  CORBA::Policy_var policy =
    server->_get_policy (RTCORBA::PRIORITY_MODEL_POLICY_TYPE);

  RTCORBA::PriorityModelPolicy_var priority_policy =
    RTCORBA::PriorityModelPolicy::_narrow (policy.in ());

  if (check_for_nil (priority_policy.in (), "PriorityModelPolicy") == -1)
    return -1;

  RTCORBA::PriorityModel priority_model =
    priority_policy->priority_model ();
  if (priority_model != RTCORBA::SERVER_DECLARED)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "ERROR: priority_model != "
                       "RTCORBA::SERVER_DECLARED!\n"),
                      -1);

  return priority_policy->server_priority ();
}

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  try
    {
      LWFT_Client_Init ci;
      CORBA::ORB_var orb = ci.init (argc, argv);

      // Initialize the ORB, resolve references and parse arguments.

      // Parse arguments.
      if (parse_args (argc, argv) != 0)
        return -1;

      // Test object 1.
      CORBA::Object_var object =
        orb->string_to_object (ior1);

      DeCoRAM::Worker_var server1 = DeCoRAM::Worker::_narrow (object.in ());
      if (check_for_nil (server1.in (), "server1") == -1)
        return -1;

      // Check that test objects are configured with SERVER_DECLARED
      // PriorityModelPolicy, and get their server priorities.

      // DeCoRAM::Worker object 1.
      CORBA::Short server1_priority =
        check_policy (server1.in ());

      if (server1_priority == -1)
        return -1;

      Client_Timer_Handler timeout_handler (iterations,
					    server_id + "-client.txt",
					    period,
					    logging);

      timeout_handler.set_orb (orb.in ());
      timeout_handler.set_worker (server1.in ());

      // add a the handler for the SIGINT signal here                                                              
      ACE_Sig_Handler sig_handler;
      sig_handler.register_handler (SIGINT, &timeout_handler);
      
      // register the timer handler with the ORB reactor
      orb->orb_core ()->reactor ()->schedule_timer (&timeout_handler,
                                                    0,
                                                    ACE_Time_Value::zero,
                                                    period);

      if (kill_me)
	{
	  std::ostringstream os;
	  os << "kill-" << ACE_OS::getpid ();
	  std::ofstream file (os.str ().c_str ());
	  file << 0;
	  file.close ();
	}

      orb->run ();

      orb->orb_core ()->reactor ()->cancel_timer (&timeout_handler);

      // DeCoRAM::Workering over. Shut down Server ORB.
      server1->stop ();
    }
  catch (const CORBA::Exception& ex)
    {
      ex._tao_print_exception (
        "Unexpected exception in Server_Declared test client:");
      return -1;
    }

  return 0;
}
