/*
; NAME:		DYCOM_RGA_P
; PURPOSE:	Returns dygon rga pressures and caches the data
; CATEGORY:	DYCOM_RGA
; CALLING SEQUENCE:	_pressure = DYCOM_RGA_P(_time)
; OUTPUTS:
;              _pressure = 2-d signal of rga data
;	
*/
FUN	PUBLIC	DYCOM_RGA_P(OPTIONAL _PATH) {
	if (!present(_PATH)) _PATH = '\\TREND::TOP.DYCOM_RGA:PRESSURE';
	if (!ALLOCATED(_DYCOM$$$RGA_LOADED)) PUBLIC _DYCOM$$$RGA_LOADED = 0q;
        if (PUBLIC _DYCOM$$$RGA_LOADED != GETNCI(_PATH,"TIME_INSERTED"))
        {
          PUBLIC _DYCOM$$$RGA_LOADED = GETNCI(_PATH,"TIME_INSERTED");
          PUBLIC _DYCOM$$$RGA_PRESSURE = EVALUATE(BUILD_PATH(_PATH));
        }
        return(PUBLIC _DYCOM$$$RGA_PRESSURE);
}
                  