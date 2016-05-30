#include "PDF.h"

void draw_divider  (HPDF_Page page, float y)
{
    HPDF_Page_MoveTo (page, 0, (HPDF_Page_GetHeight(page) - 100) - y);
    HPDF_Page_LineTo (page, 0 + 800, (HPDF_Page_GetHeight(page) - 100) - y);
    HPDF_Page_Stroke (page);
};

int WriteScriptToPDF(std::vector<std::vector<std::wstring>>& _input, HPDF_Doc& pdf, const char* font_name, std::vector<HPDF_Page>& pages, std::vector<std::vector<int>>& g_chapterDividers)
{
	int maxNoOfLines = 23; // max lines per page
	HPDF_Image logo = HPDF_LoadPngImageFromFile (pdf, "logo.png");
    HPDF_Font font;
	font = HPDF_GetFont (pdf, font_name, "CP1258");
	
	int currentPage = 1;
	int currentChapter = 1;

	// loop through every script (act?)
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
		
			// page number
			HPDF_Page_BeginText (pages.at(i));
			HPDF_Page_SetFontAndSize (pages.at(i), font, 8);
			std::string pageNumber = "Page ";
			pageNumber += std::to_string(currentPage);
			currentPage++;
			float tw = HPDF_Page_TextWidth (pages.at(i), pageNumber.c_str());
			HPDF_Page_TextOut(pages.at(i), HPDF_Page_GetWidth(pages.at(i)) - 20 - tw, HPDF_Page_GetHeight (pages.at(i)) - 15, pageNumber.c_str());
			HPDF_Page_EndText (pages.at(i));
		
			// chapter number
			HPDF_Page_BeginText (pages.at(i));
			HPDF_Page_SetFontAndSize (pages.at(i), font, 8);
			std::string chapterText = "Chapter ";
			chapterText += std::to_string(currentChapter);
			HPDF_Page_TextOut(pages.at(i), 5, HPDF_Page_GetHeight (pages.at(i)) - 15, chapterText.c_str());
			HPDF_Page_EndText (pages.at(i));

			std::wstring textBlock;
			for(int t = 0; t < maxNoOfLines; ++t)
			{
				if(((maxNoOfLines*i) + t ) < linecount) 
				{
					std::string test = "\n\n";
					std::wstring testws;
					testws.assign(test.begin(), test.end());
					for(size_t ch = 0; ch < g_chapterDividers.at(n).size(); ch++)
					{
						if( ((maxNoOfLines*i) + t) == g_chapterDividers.at(n).at(ch) )
						{
							if(((maxNoOfLines*i) + t) != 0){
								currentChapter++;
							};
							break;
						};
					};
					textBlock += _input.at(n).at( (maxNoOfLines*i) + t ) + testws;
				};
			};
			std::string temporaryS;
			temporaryS.assign(textBlock.begin(), textBlock.end());
		
			HPDF_Page_BeginText (pages.at(i));
			HPDF_Page_SetFontAndSize(pages.at(i), font, 12);
			HPDF_Page_MoveTextPos (pages.at(i), 40, HPDF_Page_GetHeight(pages.at(i)) - 80);
			HPDF_Page_SetTextLeading (pages.at(i), 12);
			HPDF_Page_TextRect (pages.at(i),
									40,
									HPDF_Page_GetHeight(pages.at(i)) - 100,
									HPDF_Page_GetWidth(pages.at(i)) - 40,
									60,
									temporaryS.c_str(),
									HPDF_TALIGN_LEFT,
									NULL);

			HPDF_Page_EndText (pages.at(i));
		};
	};
	return 0;
};