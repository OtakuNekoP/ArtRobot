#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#ifdef WIN32
    #include <fcntl.h>
#endif

#include <cairo.h>
#include <cairo-pdf.h>
//#include <cairo-ps.h>
#include <cairo-svg.h>
#include <cairo-ft.h>
#include <librsvg/rsvg.h>
#include <json-glib/json-glib.h>

#include "default.h"
#include "Color.h"
#include "Json.h"
#include "Draw.h"

Draw::Draw()
{
    //init
    this->out_file=NULL;
    this->surface_height=0;
    this->surface_width=0;

    //set
    this->inited=0;
}

int8_t Draw::init(const char *filename,const char *type,double width,double height,const char *unit,double ppi)
{
    if(this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"Init: warning: Repeat initialize!\n");
#endif
        return 1;
    }
    if(!type)
    {
#ifdef DEBUG
        fprintf(stderr,"Init: error: Unknow type , Failure to initialize!\n");
#endif
        return 3;
    }

    if(filename&&strcmp(filename,""))
    {
        this->out_file=fopen(filename,"wb");
    }
    else
    {
#ifdef WIN32
        _setmode(_fileno(stdout), O_BINARY);
#endif
        this->out_file=stdout;
    }

    this->surface_type=type;

    double scale;
    if(!strcasecmp(unit,"PX")||!strcasecmp(unit,"PT"))
    {
        this->surface_width=PT2IN(width);
        this->surface_height=PT2IN(height);
        scale=PT2IN(1);
    }
    else if(!strcasecmp(unit,"IN")||!strcasecmp(unit,"INCH"))
    {
        this->surface_width=width;
        this->surface_height=height;
        scale=1;
    }
    else if(!strcasecmp(unit,"MM"))
    {
        this->surface_width=MM2IN(width);
        this->surface_height=MM2IN(height);
        scale=MM2IN(1);
    }
    else if(!strcasecmp(unit,"CM"))
    {
        this->surface_width=MM2IN(width)*10;
        this->surface_height=MM2IN(height)*10;
        scale=MM2IN(1)*10;
    }

    if(!ppi)ppi=72;

    if(!strcasecmp(surface_type,"PDF"))
    {
        surface = cairo_pdf_surface_create_for_stream(writeCairo,(void*)this->out_file, surface_width*ppi, (surface_height)*ppi);//默认单位是mm，所以需要mm转inch
        //cairo_surface_set_fallback_resolution(surface,300,300);//设置分辨率
        cr = cairo_create (surface);//创建画笔
        cairo_scale (cr, scale*ppi, scale*ppi);//缩放画笔，因PDF用mm作为最终单位故需缩放画笔
    }
    else if(!strcasecmp(surface_type,"SVG"))
    {
        surface = cairo_svg_surface_create_for_stream(writeCairo,(void*)this->out_file, surface_width*ppi, surface_height*ppi);//默认单位pt
        cr = cairo_create (surface);//创建画笔
        cairo_scale (cr, scale*ppi, scale*ppi);
    }
    else if(!strcasecmp(surface_type,"PNG"))
    {
        surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, surface_width*ppi, surface_height*ppi);//默认单位pt
        cr = cairo_create (surface);//创建画笔
        cairo_scale (cr, scale*ppi, scale*ppi);
    }
    else
    {
#ifdef DEBUG
        fprintf(stderr,"Init: error: Unknow type , Failure to initialize!\n");
#endif
        this->inited=0;
        return 3;
    }

    this->inited=1;
    return 0;
}

int8_t Draw::uninit()
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"Uninit: warning: not initialized!\n");
#endif
        return 1;
    }

    if(!strcasecmp(surface_type,"PNG"))
    {
        cairo_surface_write_to_png_stream(surface,writeCairo,(void*)this->out_file);
    }

    cairo_destroy (cr);//回收画笔
    cairo_surface_destroy (surface);//回收介质

    fclose(this->out_file);

    this->inited=0;

    return 0;
}

int8_t Draw::nextpage()
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"NextPage: warning: not initialized!\n");
#endif
        return 1;
    }

    if(!strcasecmp(surface_type,"PDF"))
    {
        cairo_show_page(cr);
    }
    else if(!strcasecmp(surface_type,"SVG"))
    {
#ifdef DEBUG
        fprintf(stderr,"NextPage: warning: SVG surface not support multi-page,!\n");
#endif
        return 1;
    }
    else if(!strcasecmp(surface_type,"PNG"))
    {
#ifdef DEBUG
        fprintf(stderr,"NextPage: warning: PNG surface not support multi-page,!\n");
#endif
        return 1;
    }

    return 0;
}

cairo_status_t writeCairo(void * closure, const unsigned char* data, unsigned int length)
{
    fwrite(data,length,1,(FILE*)closure);
    return CAIRO_STATUS_SUCCESS;
}

int8_t Draw::draw_rectangle(Color argb, double x, double y, double width, double height)
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawRectangle: warning: not initialized!\n");
#endif
        return 1;
    }

    cairo_save(cr);//保存画笔
    cairo_set_source_rgba (cr, argb.redDouble(), argb.greenDouble(), argb.blueDouble(), argb.alphaDouble());
    cairo_rectangle(cr, x, y, width, height);
    cairo_fill(cr);
    cairo_restore(cr);//还原画笔

    return 0;
}

int8_t Draw::draw_text(const char *text, const char *fontfile, long face_index, double font_size, int8_t alignment, Color argb, double x, double y)
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawTEXT: warning: not initialized!\n");
#endif
        return 1;
    }
    if(!text)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawTEXT: warning: no text.\n");
#endif
        return 2;
    }
    if(!fontfile)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawTEXT: warning: no family.\n");
#endif
        return 3;
    }
    if(!face_index)face_index=0;

    cairo_save(cr);//保存画笔
    FT_Library ft_library;
    FT_Face ft_face;
    cairo_font_face_t *cr_face;
    if (FT_Init_FreeType (&ft_library))
    {
#ifdef DEBUG
        fprintf(stderr,"DrawTEXT: warning: FT_Init_FreeType failed.\n");
#endif
        return 4;
    }
    if (FT_New_Face (ft_library, fontfile, face_index, &ft_face))
    {
#ifdef DEBUG
        fprintf(stderr,"DrawTEXT: error: FT_New_Face failed, maybe font not found.\n");
#endif
        return 5;
    }
    cr_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
    cairo_set_font_face (cr, cr_face);
    //cairo_select_font_face (cr, family, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, font_size);
    cairo_set_source_rgba (cr, argb.redDouble(), argb.greenDouble(), argb.blueDouble(), argb.alphaDouble());

    cairo_text_extents_t extents;
    switch(alignment)
    {
    case 1:
        cairo_text_extents(cr,text,&extents);
        cairo_move_to (cr, x-extents.width/2, y);
        break;
    case 2:
        cairo_text_extents(cr,text,&extents);
        cairo_move_to (cr, x-extents.width, y);
        break;
    case 0:
    default:
        cairo_move_to (cr, x, y);
        break;
    }

    //cairo_show_text (cr, text);
    cairo_text_path (cr, text);
    cairo_fill(cr);
    cairo_restore(cr);//还原画笔

    return 0;
}

int8_t Draw::draw_svg (const char *svgfilename, double x, double y, double width, double height)
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawSVG: warning: not initialized!\n");
#endif
        return 1;
    }
    if(!this->filecheck(svgfilename))
    {
#ifdef DEBUG
        fprintf(stderr,"DrawSVG: warning: file not found: %s\n",svgfilename);
#endif
        return 2;
    }

    RsvgHandle *svg;
    svg = rsvg_handle_new_from_file(svgfilename,NULL);

    cairo_save(cr);//保存画笔

    cairo_translate (cr, x, y);
    if(width||height)
    {
        unsigned int svg_width, svg_height;
        double scaleX, scaleY;
        RsvgDimensionData dimension_data;
        rsvg_handle_get_dimensions(svg,&dimension_data);
        svg_width=dimension_data.width;
        svg_height=dimension_data.height;
        scaleX=width/(double)svg_width;
        scaleY=height/(double)svg_height;
        cairo_scale (cr, scaleX, scaleY);
    }
    rsvg_handle_render_cairo(svg, cr);

    rsvg_handle_close(svg,NULL);//释放handle
    cairo_restore(cr);//还原画笔

    return 0;
}

int8_t Draw::draw_png (const char *pngfilename, double x, double y, double width, double height)
{
    if(!this->inited)
    {
#ifdef DEBUG
        fprintf(stderr,"DrawPNG: warning: not initialized!\n");
#endif
        return 1;
    }
    if(!this->filecheck(pngfilename))
    {
#ifdef DEBUG
        fprintf(stderr,"DrawPNG: warning: file not found: %s\n",pngfilename);
#endif
        return 2;
    }

    cairo_surface_t *img=NULL;
    img=cairo_image_surface_create_from_png(pngfilename);

    cairo_save(cr);//保存画笔

    cairo_translate (cr, x, y);
    if(width||height)
    {
        unsigned int png_width, png_height;
        double scaleX, scaleY;
        png_width=cairo_image_surface_get_width(img);
        png_height=cairo_image_surface_get_height(img);
        scaleX=width/(double)png_width;
        scaleY=height/(double)png_height;
        cairo_scale (cr, scaleX, scaleY);
    }
    cairo_set_source_surface(cr,img,0,0);
    cairo_paint(cr);

    cairo_surface_destroy (img);//回收PNG介质
    cairo_restore(cr);//还原画笔

    return 0;
}

int8_t Draw::filecheck (const char *filename)
{
    FILE* file;
    file=fopen (filename, "rb");
    if(file)
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}
