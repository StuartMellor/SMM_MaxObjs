#include "ext.h"				
#include "ext_obex.h"						
#include  "libserialport.h"

typedef struct _SMM_ArduinoSerial
{
	t_object		ob;
	void			*listout;
	struct sp_port *port;
	int				error;
	char byte_buf[512];
} t_SMM_ArduinoSerial;

const char* desired_port = "COM6";

void *SMM_ArduinoSerial_new(t_symbol *s, long argc, t_atom *argv);
void SMM_ArduinoSerial_free(t_SMM_ArduinoSerial *x);
void SMM_ArduinoSerial_int(t_SMM_ArduinoSerial *x, long n);
void SMM_ArduinoSerial_float(t_SMM_ArduinoSerial *x, double f);
void SMM_ArduinoSerial_anything(t_SMM_ArduinoSerial *x, t_symbol *s, long ac, t_atom *av);
void SMM_ArduinoSerial_bang(t_SMM_ArduinoSerial *x);
void parse_serial(char *byte_buff, int byte_num);
void SMM_ArduinoSerial_initialise(t_SMM_ArduinoSerial *x);
void SMM_ArduinoSerial_sendBytes(t_SMM_ArduinoSerial *x, char* bytes, int byteN);
void SMM_ArduinoSerial_receiveBuffer(t_SMM_ArduinoSerial *x);

void list_ports();

void *SMM_ArduinoSerial_class;

void ext_main(void *r)
{
	t_class *c;

	c = class_new("SMM_ArduinoSerial", (method)SMM_ArduinoSerial_new, (method)SMM_ArduinoSerial_free, (long)sizeof(t_SMM_ArduinoSerial),
				  0L, A_GIMME, 0);

	class_addmethod(c, (method)SMM_ArduinoSerial_bang,			"bang", 0);
	class_addmethod(c, (method)SMM_ArduinoSerial_int,			"int",		A_LONG,		0);
	class_addmethod(c, (method)SMM_ArduinoSerial_float,			"float",	A_FLOAT,	0);
	class_addmethod(c, (method)SMM_ArduinoSerial_anything,		"anything",	A_GIMME,	0);
	class_addmethod(c, (method)SMM_ArduinoSerial_initialise,		"init",		0,			0);
	class_addmethod(c, (method)SMM_ArduinoSerial_receiveBuffer,	"getBuf",	0);

	class_register(CLASS_BOX, c);
	SMM_ArduinoSerial_class = c;
}

void SMM_ArduinoSerial_free(t_SMM_ArduinoSerial *x)
{
	;
}

void SMM_ArduinoSerial_int(t_SMM_ArduinoSerial *x, long n)
{
	switch (n) {
	case 250:
		SMM_ArduinoSerial_sendBytes(x, '250', 1);
		break;
	}
		
}

void SMM_ArduinoSerial_float(t_SMM_ArduinoSerial *x, double f)
{
}

void SMM_ArduinoSerial_anything(t_SMM_ArduinoSerial *x, t_symbol *s, long ac, t_atom *av)
{
}

void SMM_ArduinoSerial_bang(t_SMM_ArduinoSerial *x)
{
}



void *SMM_ArduinoSerial_new(t_symbol *s, long argc, t_atom *argv)
{
	t_SMM_ArduinoSerial *x = NULL;

	if ((x = (t_SMM_ArduinoSerial *)object_alloc(SMM_ArduinoSerial_class))) {
		x->listout = listout(x);
	}
	return (x);
}


void list_ports() {
	int i;
	struct sp_port **ports;

	int error = sp_list_ports(&ports);
	if (error == SP_OK) {
		for (i = 0; ports[i]; i++) {
			printf("Found port: '%s'\n", sp_get_port_name(ports[i]));
		}
		sp_free_port_list(ports);
	}
	else {
		printf("No serial devices detected\n");
	}
	printf("\n");
}

void parse_serial(char *byte_buff, int byte_num) {
	for (int i = 0; i < byte_num; i++) {
		post("%ld", byte_buff[i]);
	}
	post("\n");
}

void SMM_ArduinoSerial_initialise(t_SMM_ArduinoSerial *x) {
	int error = sp_get_port_by_name(desired_port, &x->port);
	if (error == SP_OK) {
		error = sp_open(x->port, SP_MODE_READ_WRITE);
		if (error == SP_OK) {
			sp_set_baudrate(x->port, 19200);
		}
	}
}


void SMM_ArduinoSerial_sendBytes(t_SMM_ArduinoSerial *x, char* bytes, int byteN) {
	sp_nonblocking_write(x->port, bytes, byteN);
}

void SMM_ArduinoSerial_receiveBuffer(t_SMM_ArduinoSerial *x) {
	int bytes_waiting = sp_input_waiting(x->port);
	if (bytes_waiting > 0) {
		post("Bytes waiting %i,  ", bytes_waiting);
		int byte_num = 0;
		byte_num = sp_nonblocking_read(x->port, x->byte_buf, 512);
		sp_flush(x->port, SP_BUF_INPUT);
	}
}