
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "hpdf.h"

#include <string>
#include <vector>

int WriteScriptToPDF(std::vector<std::vector<std::string>>& _input, HPDF_Doc& pdf, const char* font_name, std::vector<HPDF_Page>& pages);