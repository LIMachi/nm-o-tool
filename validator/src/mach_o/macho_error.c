#include <mach_o_validator.h>

#include <stdio.h>

t_validator_error	macho_error(t_macho_file *mo, const t_validator_error err,
							const t_debug_tuple debug_tuple)
{
	if (err != VE_OK)
		dprintf(2, "Caught error(%d) in macho validation, please set a breakpoi"
				"nt\nCaused by %s:%s:%d\n", err, debug_tuple.file,
				debug_tuple.function, debug_tuple.line);
	if (mo != NULL)
		mo->err = err;
	return (err);
}
