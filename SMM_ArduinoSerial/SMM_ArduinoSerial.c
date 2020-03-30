#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include  "libserialport.h"

typedef struct _betterSerial
{
	t_object		ob;
	t_atom			val;
	t_symbol		*name;
	void			*out;
	struct sp_port *port;
	int				error;
	char byte_buf[512];
} t_betterSerial;

const char* desired_port = "COM6";

void *betterSerial_new(t_symbol *s, long argc, t_atom *argv);
void betterSerial_free(t_betterSerial *x);
void betterSerial_int(t_betterSerial *x, long n);
void betterSerial_float(t_betterSerial *x, double f);
void betterSerial_anything(t_betterSerial *x, t_symbol *s, long ac, t_atom *av);
void betterSerial_bang(t_betterSerial *x);
void parse_serial(char *byte_buff, int byte_num);
void betterSerial_initialise(t_betterSerial *x);
void betterSerial_sendBytes(t_betterSerial *x);
void betterSerial_receiveBuffer(t_betterSerial *x);

void list_ports();

void *betterSerial_class;

void ext_main(void *r)
{
	t_class *c;

	c = class_new("betterSerial", (method)betterSerial_new, (method)betterSerial_free, (long)sizeof(t_betterSerial),
				  0L, A_GIMME, 0);

	class_addmethod(c, (method)betterSerial_bang,			"bang", 0);
	class_addmethod(c, (method)betterSerial_int,			"int",		A_LONG,		0);
	class_addmethod(c, (method)betterSerial_float,			"float",	A_FLOAT,	0);
	class_addmethod(c, (method)betterSerial_anything,		"anything",	A_GIMME,	0);
	class_addmethod(c, (method)betterSerial_initialise,		"init",		0,			0);
	class_addmethod(c, (method)betterSerial_receiveBuffer,	"getBuf",	0);

	class_register(CLASS_BOX, c);
	betterSerial_class = c;
}

void betterSerial_free(t_betterSerial *x)
{
	;
}

void betterSerial_int(t_betterSerial *x, long n)
{
	switch (n) {
	case 250:
		betterSerial_sendBytes(x, '250', 1);
		break;
	}
		
}

void betterSerial_float(t_betterSerial *x, double f)
{
}

void betterSerial_anything(t_betterSerial *x, t_symbol *s, long ac, t_atom *av)
{
}

void betterSerial_bang(t_betterSerial *x)
{
}



void *betterSerial_new(t_symbol *s, long argc, t_atom *argv)
{
	t_betterSerial *x = NULL;

	if ((x = (t_betterSerial *)object_alloc(betterSerial_class))) {

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

void betterSerial_initialise(t_betterSerial *x) {
	int error = sp_get_port_by_name(desired_port, &x->port);
	if (error == SP_OK) {
		error = sp_open(x->port, SP_MODE_READ_WRITE);
		if (error == SP_OK) {
			sp_set_baudrate(x->port, 19200);
		}
	}
}


void betterSerial_sendBytes(t_betterSerial *x, char* bytes, int byteN) {
	sp_nonblocking_write(x->port, bytes, byteN);
}

void betterSerial_receiveBuffer(t_betterSerial *x) {
	int bytes_waiting = sp_input_waiting(x->port);
	if (bytes_waiting > 0) {
		post("Bytes waiting %i,  ", bytes_waiting);
		int byte_num = 0;
		byte_num = sp_nonblocking_read(x->port, x->byte_buf, 512);
		sp_flush(x->port, SP_BUF_INPUT);
	}
}