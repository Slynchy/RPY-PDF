#include "PDF.h"

int WriteScriptToPDF(std::vector<std::vector<std::string>>& _input, HPDF_Doc& pdf, const char* font_name, std::vector<HPDF_Page>& pages)
{
	int maxNoOfLines = 25; // max lines per page
	HPDF_Image logo = HPDF_LoadPngImageFromFile (pdf, "logo.png");
    HPDF_Font font;
	font = HPDF_GetFont (pdf, font_name, "CP1250");
	
	int currentPage = 1;
	
	for(size_t n = 0; n < _input.size(); ++n)
	{
		int linecount = int(_input.at(n).size());
		int pagecount = (linecount / maxNoOfLines) + 1;
		for(int i = 0; i < pagecount; ++i)
		{
			HPDF_Page tempPage;
			pages.push_back(tempPage);
			pages.at(i) = HPDF_AddPage (pdf);
			HPDF_Page_DrawImage (pages.at(i), logo, 
				(HPDF_Page_GetWidth(pages.at(i)) - HPDF_Image_GetWidth (logo)) / 2, 
				HPDF_Page_GetHeight (pages.at(i)) - HPDF_Image_GetHeight (logo) - 10, 
				HPDF_Image_GetWidth (logo),
				HPDF_Image_GetHeight (logo));
		
			HPDF_Page_BeginText (pages.at(i));
			HPDF_Page_SetFontAndSize (pages.at(i), font, 8);
			std::string pageNumber = "Page ";
			pageNumber += std::to_string(currentPage);
			currentPage++;
			float tw = HPDF_Page_TextWidth (pages.at(i), pageNumber.c_str());
			HPDF_Page_TextOut(pages.at(i), HPDF_Page_GetWidth(pages.at(i)) - 20 - tw, HPDF_Page_GetHeight (pages.at(i)) - 15, pageNumber.c_str());
			HPDF_Page_EndText (pages.at(i));
		
			HPDF_Page_BeginText (pages.at(i));
			HPDF_Page_SetFontAndSize(pages.at(i), font, 12);
			HPDF_Page_MoveTextPos (pages.at(i), 40, HPDF_Page_GetHeight(pages.at(i)) - 80);
			HPDF_Page_SetTextLeading (pages.at(i), 12);

			std::string textBlock = "";
			for(int t = 0; t < maxNoOfLines; ++t)
			{
				//HPDF_Page_ShowTextNextLine (pages.at(i),
				//	_input.at( (maxNoOfLines*i) + t ).c_str());
				if(((maxNoOfLines*i) + t ) < linecount) textBlock += _input.at(n).at( (maxNoOfLines*i) + t ) + "\n\n";
			};
			HPDF_Page_TextRect (pages.at(i),
									40,
									HPDF_Page_GetHeight(pages.at(i)) - 100,
									HPDF_Page_GetWidth(pages.at(i)) - 40,
									60,
									textBlock.c_str(),
									HPDF_TALIGN_LEFT,
									NULL);

			HPDF_Page_EndText (pages.at(i));
		};
	};
	return 0;
};