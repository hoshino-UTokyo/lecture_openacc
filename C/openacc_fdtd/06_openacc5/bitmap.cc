/**
 * @file bitmap.cc
 * @brief Class for Bitmap format file
 *
 * Class for Bitmap format file
 *
 * @author Takashi Shimokawabe
 * @date 2010/12/12 Created
 * @version 0.1.0
 *
 */

#include "bitmap.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "bitmap_palette-def.h"

BitmapHeader::BitmapHeader()
{
    is_bitmap_ = false;
    init();
}

BitmapHeader::~BitmapHeader()
{
    delete [] palette_;
    palette_ = NULL;
}

void BitmapHeader::init()
{
    // Bitmap File Header (14 byte)
    strncpy((char *)&bfType_, "BM", 2);
    bfSize_      = 0;                // must be modified
    bfReserved1_ = 0;
    bfReserved2_ = 0;
    bfOffBits_   = 14 + 40;          // may be modified

    // Bitmap Information Header (40 byte)
    biSize_         = 40;
    biWidth_        = 0;             // must be modified
    biHeight_       = 0;             // must be modified
    biPlanes_       = 1;
    biBitCount_     = 24;            // may be modified
    biCompression_  = 0;
    biSizeImage_    = 0;             // must be modified
    biXPixPerMeter_ = 0;
    biYPixPerMeter_ = 0;
    biClrUsed_      = 0;
    biCirImportant_ = 0;

    // Bitmap Palette Data (RGBQUAD)
    palette_ = NULL;                 // may be modified
}

bool BitmapHeader::set(int width, int height, int bit_count, const unsigned char *palette)
{
    is_bitmap_ = false;
    if (width_height_are_supported(width, height) == false) return false;
    
    int ret = bit_count_is_supported(bit_count);
    if ( ! ((ret == 1 && palette) || (ret == 2 && palette == NULL)) ) return false;
    
    biWidth_     = width;
    biHeight_    = height;    
    biBitCount_  = bit_count;
    biSizeImage_ = abs(height) * linesize();
    bfOffBits_   = 14 + 40 + palette_size();
    bfSize_      = biSizeImage_ + bfOffBits_;

    const size_t n = palette_size();   
    if (n > 0) {
        delete [] palette_;
        palette_ = new unsigned char[n];
        if (palette_ == NULL) return false;
        memcpy(palette_, palette, sizeof(unsigned char)*n);
    }
    
    is_bitmap_ = true;
    return true;
}

bool BitmapHeader::read(FILE *fp)
{
    if (fp == NULL) return false;
    is_bitmap_ = false;
    
    // read Bitmap File Header
    if ((fread(&bfType_, sizeof(bfType_), 1, fp) != 1) ||
        (strncmp((const char*)&bfType_, "BM", 2) != 0)) { return false; }

    if (fread(&bfSize_,      sizeof(bfSize_),      1, fp) != 1) { return false; }
    if (fread(&bfReserved1_, sizeof(bfReserved1_), 1, fp) != 1) { return false; }
    if (fread(&bfReserved2_, sizeof(bfReserved2_), 1, fp) != 1) { return false; }
    if (fread(&bfOffBits_,   sizeof(bfOffBits_),   1, fp) != 1) { return false; }

    // read Bitmap Information Header
    if ((fread(&biSize_,   sizeof(biSize_),   1, fp) != 1) || (biSize_ != 40)) { return false; }    
    if ( fread(&biWidth_,  sizeof(biWidth_),  1, fp) != 1) { return false; }
    if ( fread(&biHeight_, sizeof(biHeight_), 1, fp) != 1) { return false; }
    if ((fread(&biPlanes_, sizeof(biPlanes_), 1, fp) != 1) || (biPlanes_!= 1)) { return false; }
    
    if ((fread(&biBitCount_, sizeof(biBitCount_), 1, fp) != 1) ||
        (bit_count_is_supported(biBitCount_) == 0)) { return false; }
    
    if ((fread(&biCompression_,  sizeof(biCompression_),  1, fp) != 1) || (biCompression_ != 0)) { return false; }
    if ( fread(&biSizeImage_,    sizeof(biSizeImage_),    1, fp) != 1) { return false; }
    if ( fread(&biXPixPerMeter_, sizeof(biXPixPerMeter_), 1, fp) != 1) { return false; }
    if ( fread(&biYPixPerMeter_, sizeof(biYPixPerMeter_), 1, fp) != 1) { return false; }
    if ((fread(&biClrUsed_,      sizeof(biClrUsed_),      1, fp) != 1) || (biClrUsed_ != 0)) { return false; }
    if ((fread(&biCirImportant_, sizeof(biCirImportant_), 1, fp) != 1) || (biCirImportant_ != 0)) { return false; }
    
    const size_t n = palette_size();
    if (n > 0) {
        delete [] palette_;
        palette_ = new unsigned char[n];
        if (palette_ == NULL) return false;
        if (fread(palette_, sizeof(unsigned char), n, fp) != n) { return false; }
    }
    
    is_bitmap_ = true;
    
    return true;
}

bool BitmapHeader::write(FILE *fp) const
{
    if (! is_bitmap()) return false;
    if (fp == NULL) return false;
                           
    // Bitmap File Header (14 byte)
    fwrite(&bfType_,      sizeof(bfType_),      1, fp);
    fwrite(&bfSize_,      sizeof(bfSize_),      1, fp);
    fwrite(&bfReserved1_, sizeof(bfReserved1_), 1, fp);
    fwrite(&bfReserved2_, sizeof(bfReserved2_), 1, fp);
    fwrite(&bfOffBits_,   sizeof(bfOffBits_),   1, fp);

    // Bitmap Information Header (40 byte)
    fwrite(&biSize_,         sizeof(biSize_),         1, fp);
    fwrite(&biWidth_,        sizeof(biWidth_),        1, fp);
    fwrite(&biHeight_,       sizeof(biHeight_),       1, fp);
    fwrite(&biPlanes_,       sizeof(biPlanes_),       1, fp);
    fwrite(&biBitCount_,     sizeof(biBitCount_),     1, fp);
    fwrite(&biCompression_,  sizeof(biCompression_),  1, fp);
    fwrite(&biSizeImage_,    sizeof(biSizeImage_),    1, fp);
    fwrite(&biXPixPerMeter_, sizeof(biXPixPerMeter_), 1, fp);
    fwrite(&biYPixPerMeter_, sizeof(biYPixPerMeter_), 1, fp);
    fwrite(&biClrUsed_,      sizeof(biClrUsed_),      1, fp);
    fwrite(&biCirImportant_, sizeof(biCirImportant_), 1, fp);

    // Bitmap Palette Data (RGBQUAD)
    if (palette_size() > 0)
        fwrite(palette_, sizeof(unsigned char), palette_size(), fp);

    return true;
}

int BitmapHeader::width() const              { return biWidth_; }
int BitmapHeader::height() const             { return biHeight_; }
int BitmapHeader::bit_count() const          { return biBitCount_; }
size_t BitmapHeader::image_size() const      { return biSizeImage_; }
size_t BitmapHeader::image_offset() const    { return bfOffBits_; }
size_t BitmapHeader::total_data_size() const { return bfSize_; }

size_t BitmapHeader::linesize() const
{
    size_t byte = (width()*bit_count() + 8 - 1) / 8;
    return ((byte + 4 - 1) / 4) * 4;
}

size_t BitmapHeader::palette_size() const
{
    const size_t palette_elem_size = 4; // byte
    if      (bit_count() == 1) return   2 * palette_elem_size;
    else if (bit_count() == 4) return  16 * palette_elem_size;
    else if (bit_count() == 8) return 256 * palette_elem_size;
    else                     return 0;
}

bool BitmapHeader::has_palette() const
{
    return (palette_size() > 0 ? true : false);
}

bool BitmapHeader::is_fullcolor() const { return (! has_palette()); }
bool BitmapHeader::is_bitmap() const { return is_bitmap_; }    
const unsigned char *BitmapHeader::palette() const
{
    return has_palette() ? palette_ : NULL;
}

int BitmapHeader::bit_count_is_supported(int bit_count) const
{
    if      (bit_count == 8)                     return 1;
    else if (bit_count == 24 || bit_count == 32) return 2;
    else                                         return 0;
}

bool BitmapHeader::width_height_are_supported(int width, int height) const
{ 
    if (width <= 0 || height == 0) return false;
    else                           return true;
}








BitmapBase::BitmapBase()
    : file_(NULL), header_(NULL), error_(0)
{
    header_ = new BitmapHeader();
}

BitmapBase::BitmapBase(const char *file)
    : file_(NULL), header_(NULL), error_(0)
{
    header_ = new BitmapHeader();
    open(file);
}

BitmapBase::~BitmapBase()
{
    delete header_;
    header_ = NULL;
}

bool BitmapBase::open(const char *file)
{
    if (file == NULL) {
        set_error();
        return false;
    }
    file_ = file;
    return true;
}

const char *BitmapBase::file_name() const { return file_; }

int    BitmapBase::width()             const { return header_->width()          ; }
int    BitmapBase::height()            const { return header_->height()         ; }
int    BitmapBase::bit_count()         const { return header_->bit_count()      ; }
size_t BitmapBase::image_data_size()   const { return header_->image_size()     ; } // rename
size_t BitmapBase::image_data_offset() const { return header_->image_offset()   ; } // rename
size_t BitmapBase::size()              const { return header_->total_data_size(); } // rename
size_t BitmapBase::linesize()          const { return header_->linesize()       ; } 
size_t BitmapBase::palette_size()      const { return header_->palette_size()   ; }
bool   BitmapBase::has_palette()       const { return header_->has_palette()    ; }
bool   BitmapBase::is_fullcolor()      const { return header_->is_fullcolor()   ; }
bool   BitmapBase::is_bitmap()         const { return header_->is_bitmap()      ; }    
const unsigned char *BitmapBase::palette() const { return header_->palette(); }


BitmapBase::operator void *() const
{
    return is_failure() ? 0 : const_cast<BitmapBase*>(this);
}

bool BitmapBase::operator !() const
{
    return is_failure();
}

bool BitmapBase::is_good() const
{
    return !is_failure();
}

bool BitmapBase::is_failure() const
{
    return (error_ == 0 ? false : true);
}

void BitmapBase::set_error(int error)
{
    error_ = error;
}

void BitmapBase::clear_error()
{
    error_ = 0;
}



// functions for BitmapWriter, BitmapReader, BitmapPalette, RGBPalette
namespace {

template <typename T>
inline unsigned char f2u(T f)
{
    return (unsigned char)(fmin(fmax(f, 0.0), 1.0)*255);
}
    
    
void pixel2rgb(const unsigned char *p,
               unsigned char *r, unsigned char *g, unsigned char *b)
{

    *b = *p;
    *g = *(p+1);    
    *r = *(p+2);
}

    
template <typename T>
void pixel2rgb(const unsigned char *p, T *r, T *g, T *b)
{   
    *b = (* p   ) / 255.0;
    *g = (*(p+1)) / 255.0;
    *r = (*(p+2)) / 255.0;
}

template void pixel2rgb(const unsigned char *p, float *r, float *g, float *b);
template void pixel2rgb(const unsigned char *p, double *r, double *g, double *b);

void rgb2pixel(const unsigned char *r, const unsigned char *g, const unsigned char *b,
               unsigned char *p)
{
    *p     = *b;    
    *(p+1) = *g;    
    *(p+2) = *r;
}

template <typename T>
void rgb2pixel(const T *r, const T *g, const T *b, unsigned char *p)
{
//     *p     = (unsigned char)(fmin(fmax((*b), 0.0), 1.0)*255);
//     *(p+1) = (unsigned char)(fmin(fmax((*g), 0.0), 1.0)*255);
//     *(p+2) = (unsigned char)(fmin(fmax((*r), 0.0), 1.0)*255);    

//     *p     = *b <= 0.0 ? 0 : (*b >= 1.0 ? 255 : (unsigned char)(*b*255));
//     *(p+1) = *g <= 0.0 ? 0 : (*g >= 1.0 ? 255 : (unsigned char)(*g*255));
//     *(p+2) = *r <= 0.0 ? 0 : (*r >= 1.0 ? 255 : (unsigned char)(*r*255));

//     const int bi = (int)(*b * 255); *p     = bi < 0 ? 0 : (bi > 255 ? 255 : (unsigned char)bi);
//     const int gi = (int)(*g * 255); *(p+1) = gi < 0 ? 0 : (gi > 255 ? 255 : (unsigned char)gi);
//     const int ri = (int)(*r * 255); *(p+2) = ri < 0 ? 0 : (ri > 255 ? 255 : (unsigned char)ri);
    
    *p     = f2u(*b);
    *(p+1) = f2u(*g);
    *(p+2) = f2u(*r);
}

template void rgb2pixel(const float *r, const float *g, const float *b, unsigned char *p);
template void rgb2pixel(const double *r, const double *g, const double *b, unsigned char *p);



void pixelline2array(int n, const unsigned char *p, unsigned char *a)
{
    memcpy(a, p, n);
}

template <typename T>
void pixelline2array(int n, const unsigned char *p, T *a)
{
    for (int i=0; i<n; i++) { a[i] = p[i] / 255.0; }
}

template void pixelline2array(int n, const unsigned char *p, float *a);
template void pixelline2array(int n, const unsigned char *p, double *a);



void array2pixelline(int n, const unsigned char *a, unsigned char *p)
{
    memcpy(p, a, n);
}

template <typename T>
void array2pixelline(int n, const T *a, unsigned char *p)
{
    for (int i=0; i<n; i++) {
        p[i] = (unsigned char)(fmin(fmax((a[i]), 0.0), 1.0)*255);
    }
}

template void array2pixelline(int n, const float *a, unsigned char *p);
template void array2pixelline(int n, const double *a, unsigned char *p);


} // namespace




BitmapWriter::BitmapWriter()
    : BitmapBase()
{
}

BitmapWriter::BitmapWriter(const char *file)
    : BitmapBase(file)
{
}

BitmapWriter::~BitmapWriter()
{
}

template int BitmapWriter::write_rgb(int,  int ,
                                     const unsigned char *, const unsigned char *, const unsigned char *,
                                     int);
template int BitmapWriter::write_rgb(int,  int ,
                                     const float *, const float *, const float *,
                                     int);
template int BitmapWriter::write_rgb(int,  int ,
                                     const double *, const double *, const double *,
                                     int);

template <typename T>
int BitmapWriter::write_rgb(int width, int height,
                            const T *r, const T *g, const T *b, int bit_count)
{
    if (is_failure()) return -2;

    if (header_->bit_count_is_supported(bit_count)         != 2    ) return -3;
    if (header_->width_height_are_supported(width, height) == false) return -4;
    
    header_->set(width, height, bit_count, NULL);

    FILE *fp = fopen(file_, "w");
    if (fp == NULL) return -5;

    header_->write(fp);

    // write data
    size_t len = header_->linesize();

    unsigned char* line = new unsigned char[len];
    if (line == NULL) return -1;
    memset(line, 0, len);

    const int h = abs(height);
    const int w = width;

    const int bc = header_->bit_count() / 8; // 3 or 4
    for (int j=0; j<h; j++) {
        for (int i=0; i<w; i++) {            
            const int id = j*w + i;
            rgb2pixel(&r[id], &g[id], &b[id], &line[bc*i]);
        }
        fwrite(line, sizeof(unsigned char), len, fp);
    }

    delete [] line; line = NULL;
    fclose(fp);
    return w*h;
}

//template int BitmapWriter::write_rgb(int , int , const unsigned char *, const RGBPalette &, int);
template int BitmapWriter::write_rgb(int , int , const float *, float, float,  const RGBPalette &, int);
template int BitmapWriter::write_rgb(int , int , const double *, double, double, const RGBPalette &, int);

template <typename T>
int BitmapWriter::write_rgb(int width, int height, const T *p, T pmin, T pmax,
                            const RGBPalette &palette, int bit_count)
{
    if (is_failure()) return -2;

    if (header_->bit_count_is_supported(bit_count)         != 2    ) return -3;
    if (header_->width_height_are_supported(width, height) == false) return -4;
    
    const int n = width * height;
    T *r = new T[n];
    T *g = new T[n];
    T *b = new T[n];

    for (int i=0; i<n; i++) palette.rgb(p[i], pmin, pmax, &r[i], &g[i], &b[i]);

    int ret = write_rgb(width, height, r, g, b, bit_count);

    delete [] r; r = NULL;
    delete [] g; g = NULL;
    delete [] b; b = NULL;

    return ret;
}

//template int BitmapWriter::write_rgb(int , int , const unsigned char *, const RGBPalette &, int);
template int BitmapWriter::write_rgb(int , int , const float *, const RGBPalette &, int);
template int BitmapWriter::write_rgb(int , int , const double *, const RGBPalette &, int);

template <typename T>
int BitmapWriter::write_rgb(int width, int height, const T *p, const RGBPalette &palette, int bit_count)
{
    if (is_failure()) return -2;

    if (header_->bit_count_is_supported(bit_count)         != 2    ) return -3;
    if (header_->width_height_are_supported(width, height) == false) return -4;
    
    const int n = width * height;
    T pmin = p[0];
    T pmax = p[0];
    for (int i=0; i<n; i++) {
        pmin = fmin(p[i], pmin);
        pmax = fmax(p[i], pmax);
    }

    return write_rgb(width, height, p, pmin, pmax, palette, bit_count);
}


template int BitmapWriter::write_8bit(int , int , const unsigned char *, const unsigned char *);
template int BitmapWriter::write_8bit(int , int , const float *, const unsigned char *);
template int BitmapWriter::write_8bit(int , int , const double *, const unsigned char *);

template <typename T>
int BitmapWriter::write_8bit(int width, int height, const T *p, const unsigned char *palette)
{
    if (is_failure()) return -2;

    int bit_count = 8;
    if (header_->width_height_are_supported(width, height) == false) return -4;
    
    header_->set(width, height, bit_count, palette);

    FILE *fp = fopen(file_, "w");
    if (fp == NULL) return -5;

    header_->write(fp);

    // write data
    size_t len = header_->linesize();

    unsigned char* line = new unsigned char[len];
    if (line == NULL) return -1;
    memset(line, 0, len);

    const int h = abs(height);
    const int w = width;

    for (int j=0; j<h; j++) {
        array2pixelline(w, &p[j*w], line);
        fwrite(line, sizeof(unsigned char), len, fp);    
    }

    delete [] line; line = NULL;
    fclose(fp);
    return w*h;
}


template int BitmapWriter::write_8bit(int , int , const unsigned char *, const BitmapPalette &);
template int BitmapWriter::write_8bit(int , int , const float *, const BitmapPalette &);
template int BitmapWriter::write_8bit(int , int , const double *, const BitmapPalette &);

template <typename T>
int BitmapWriter::write_8bit(int width, int height, const T *p, const BitmapPalette &palette)
{
    return write_8bit(width, height, p, palette.data());
}




BitmapReader::BitmapReader()
    : BitmapBase()
{   
}

BitmapReader::BitmapReader(const char *file)
    : BitmapBase(file)
{
    open(file);
}

BitmapReader::~BitmapReader()
{
}

bool BitmapReader::open(const char *file)
{
    if (is_failure()) return false;
    if (! read_header()) {
        set_error();
        return false;
    }
    return true;
}

bool BitmapReader::read_header()
{
    if (is_failure()) return false;

    FILE *fp = fopen(file_, "r");
    if (fp == NULL) return false;
    
    header_->read(fp);

    fclose(fp); fp = NULL;
    return true;
}

template int BitmapReader::read_rgb(int, unsigned char *, unsigned char *, unsigned char *);
template int BitmapReader::read_rgb(int, float *, float *, float *);
template int BitmapReader::read_rgb(int, double *, double *, double *);

template <typename T>
int BitmapReader::read_rgb(int size, T *r, T *g, T *b)
{
    if (is_failure()) return -2;
    if (! header_->is_bitmap()) return -4;
    
    if (! header_->is_fullcolor()) return -3;
    
    const int h = abs(header_->height());
    const int w = header_->width();

    if (size < w*h) return 0;

    FILE *fp = fopen(file_, "r");
    if (fp == NULL) return -5;

    fseek(fp, header_->image_offset(), SEEK_SET);

    size_t len = header_->linesize();
    unsigned char* line = new unsigned char[len];

    const int bc = header_->bit_count()/8; // 3 or 4
    for (int j=0; j<h; j++) {
        fread(line, sizeof(unsigned char), len, fp);
        for (int i=0; i<w; i++) {
            const int id = j*w + i;
            pixel2rgb(&line[bc*i], &r[id], &g[id], &b[id]);
        }
    }

    delete [] line;
    fclose(fp);

    return w*h;
}


template int BitmapReader::read_8bit(int, unsigned char *, unsigned char *);
template int BitmapReader::read_8bit(int, float *, unsigned char *);
template int BitmapReader::read_8bit(int, double *, unsigned char *);

template <typename T>
int BitmapReader::read_8bit(int size, T *p, unsigned char *palette)
{
    if (is_failure()) return -2;
    if (! header_->is_bitmap()) return -4;
    
    if (! header_->has_palette()) return -3;
    
    const int h = abs(header_->height());
    const int w = header_->width();

    if (size < w*h) return 0;

    if (palette) memcpy(palette, header_->palette(), palette_size());
    
    FILE *fp = fopen(file_, "r");
    if (fp == NULL) return -5;

    fseek(fp, header_->image_offset(), SEEK_SET);

    size_t len = header_->linesize();
    unsigned char* line = new unsigned char[len];

    for (int j=0; j<h; j++) {
        fread(line, sizeof(unsigned char), len, fp);
        pixelline2array(w, line, &p[j*w]);
    }

    delete [] line;
    fclose(fp);

    return w*h;
}







BitmapPalette::BitmapPalette()
    : data_(NULL), size_(256 * 4)
{
    make(RAINBOW);
}

BitmapPalette::BitmapPalette(PALETTE palette)
    : data_(NULL), size_(256 * 4)
{
    make(palette);
}

BitmapPalette::~BitmapPalette()
{
    delete [] data_;
    data_ = NULL;
}
    
bool BitmapPalette::make(PALETTE palette)
{
    if (data_ == NULL) data_ = new unsigned char[size_];
    if (data_ == NULL) return false;

    if (palette == RAINBOW) {
        const unsigned char d[] = { BITMAP_PALETTE_RAINBOW };
        memcpy(data_, d, size_);
    } else if (palette == SEISMIC) {
        const unsigned char d[] = { BITMAP_PALETTE_SEISMIC };
        memcpy(data_, d, size_);
    } else if (palette == HOT) {
        RGBPalette rgbpalette(RGBPalette::HOT);
        rgbpalette.generate_bitmap_rgbquad_palette(256, data_);
    }

    return true;
}

const unsigned char *BitmapPalette::data() const { return data_; }
size_t BitmapPalette::size() const { return size_; }

    


RGBPalette::RGBPalette()
{
}

RGBPalette::RGBPalette(PRESET preset)
{
    set(preset);
}

RGBPalette::~RGBPalette()
{
}


int RGBPalette::add(double value, double r, double g, double b)
{
    struct RGB rgb = {r, g, b};
    rgbmap.insert(std::pair<double, struct RGB>(value, rgb));
    return rgbmap.size();
}

void RGBPalette::set(PRESET preset)
{
    clear();
    if      (preset == BLUEHOT         ) set_bluehot();
    else if (preset == CALEBLACK       ) set_caleblack();
    else if (preset == CALEWHITE       ) set_calewhite();
    else if (preset == CONTOURED       ) set_contoured();
    else if (preset == CPK_JMOL        ) set_cpk_jmol();
    else if (preset == CPK_RASMOL      ) set_cpk_rasmol();
    else if (preset == DIFFERENCE      ) set_difference();
    else if (preset == GRAY            ) set_gray();
    else if (preset == HOT             ) set_hot();
    else if (preset == HOT_AND_COLD    ) set_hot_and_cold();
    else if (preset == HOT_DESATURATED ) set_hot_desaturated();
    else if (preset == LEVELS          ) set_levels();
    else if (preset == ORANGEHOT       ) set_orangehot();
    else if (preset == RAINBOW         ) set_rainbow();
    else if (preset == XRAY            ) set_xray();    
}

void RGBPalette::clear()
{
    rgbmap.clear();
}

template void RGBPalette::rgb(double , double, double, float *, float *, float *) const;
template void RGBPalette::rgb(double , double, double, double *, double *, double *) const;

template <typename T>
void RGBPalette::rgb(double value, double vmin, double vmax, T *r, T *g, T *b) const
{
    value = (value - vmin) / (vmax - vmin);
    RGBmap::const_iterator it1 = rgbmap.lower_bound(value);
    if (it1 == rgbmap.begin()) {
        *r = it1->second.r;
        *g = it1->second.g;
        *b = it1->second.b;
        return;
    } else if (it1 == rgbmap.end()) {
        it1--;
        *r = it1->second.r;
        *g = it1->second.g;
        *b = it1->second.b;
        return;
    }
    RGBmap::const_iterator it0 = it1;
    it0--;
    
    const double s = (value - it0->first) / (it1->first - it0->first);
    const double t = 1.0 - s;
    *r = t * it0->second.r + s * it1->second.r;
    *g = t * it0->second.g + s * it1->second.g;
    *b = t * it0->second.b + s * it1->second.b;
}
    
void RGBPalette::generate_bitmap_rgbquad_palette(int nstep, unsigned char *palette) const
{
    double vmin = 0.0;
    double vmax = nstep - 1;

    for (int i=0; i<nstep; i++) {
        double r, g, b;
        rgb(i, vmin, vmax, &r, &g, &b);
        palette[4*i  ] = (unsigned char)(fmin(fmax(b, 0.0), 1.0)*255);
        palette[4*i+1] = (unsigned char)(fmin(fmax(g, 0.0), 1.0)*255);
        palette[4*i+2] = (unsigned char)(fmin(fmax(r, 0.0), 1.0)*255);
        palette[4*i+3] = 0;
    }
}
    

void RGBPalette::set_bluehot()
{
    add(0.000, 0.0000, 0.0000, 0.0000);
    add(0.333, 0.0000, 0.0000, 0.4980);
    add(0.666, 0.0000, 0.4980, 1.0000);
    add(1.000, 1.0000, 1.0000, 1.0000);
}

void RGBPalette::set_caleblack()
{    
    add(0.000, 0.0000, 0.0000, 0.0000);
    add(0.170, 0.0000, 0.0000, 1.0000);
    add(0.340, 0.0000, 1.0000, 1.0000);
    add(0.500, 0.0000, 1.0000, 0.0000);
    add(0.670, 1.0000, 1.0000, 0.0000);
    add(0.840, 1.0000, 0.0000, 0.0000);
    add(1.000, 1.0000, 0.0000, 1.0000);
}

void RGBPalette::set_calewhite()
{
    add(0.000, 1.0000, 1.0000, 1.0000);
    add(0.170, 0.0000, 0.0000, 1.0000);
    add(0.340, 0.0000, 1.0000, 1.0000);
    add(0.500, 0.0000, 1.0000, 0.0000);
    add(0.670, 1.0000, 1.0000, 0.0000);
    add(0.840, 1.0000, 0.0000, 0.0000);
    add(1.000, 1.0000, 0.0000, 1.0000);
};

void RGBPalette::set_contoured()
{
    add(0.000, 0.0000, 0.0000, 1.0000);
    add(0.333, 0.0000, 1.0000, 0.0000);
    add(0.666, 1.0000, 1.0000, 0.0000);
    add(1.000, 1.0000, 0.0000, 0.0000);
}

void RGBPalette::set_cpk_jmol()
{
    add(0.000, 1.0000, 1.0000, 1.0000);
    add(0.009, 0.8510, 1.0000, 1.0000);
    add(0.019, 0.8000, 0.5020, 1.0000);
    add(0.028, 0.7608, 1.0000, 0.0000);
    add(0.037, 1.0000, 0.7098, 0.7098);
    add(0.046, 0.5647, 0.5647, 0.5647);
    add(0.056, 0.1843, 0.3137, 0.9725);
    add(0.065, 1.0000, 0.0510, 0.0510);
    add(0.074, 0.5647, 0.8745, 0.3137);
    add(0.083, 0.7020, 0.8863, 0.9608);
    add(0.093, 0.6706, 0.3608, 0.9451);
    add(0.102, 0.5373, 1.0000, 0.0000);
    add(0.111, 0.7451, 0.6510, 0.6510);
    add(0.120, 0.9373, 0.7804, 0.6235);
    add(0.130, 1.0000, 0.5020, 0.0000);
    add(0.139, 1.0000, 1.0000, 0.1843);
    add(0.148, 0.1216, 0.9373, 0.1216);
    add(0.157, 0.5020, 0.8196, 0.8863);
    add(0.167, 0.5608, 0.2510, 0.8275);
    add(0.176, 0.2353, 1.0000, 0.0000);
    add(0.185, 0.9020, 0.9020, 0.9020);
    add(0.194, 0.7451, 0.7608, 0.7765);
    add(0.204, 0.6510, 0.6510, 0.6706);
    add(0.213, 0.5373, 0.6000, 0.7765);
    add(0.222, 0.6118, 0.4745, 0.7765);
    add(0.231, 0.8745, 0.4000, 0.2000);
    add(0.241, 0.9373, 0.5647, 0.6235);
    add(0.250, 0.3137, 0.8157, 0.3137);
    add(0.259, 0.7804, 0.5020, 0.2000);
    add(0.269, 0.4863, 0.5020, 0.6863);
    add(0.278, 0.7608, 0.5608, 0.5608);
    add(0.287, 0.4000, 0.5608, 0.5608);
    add(0.296, 0.7373, 0.5020, 0.8863);
    add(0.306, 1.0000, 0.6275, 0.0000);
    add(0.315, 0.6510, 0.1608, 0.1608);
    add(0.324, 0.3608, 0.7216, 0.8196);
    add(0.333, 0.4353, 0.1765, 0.6863);
    add(0.343, 0.0000, 1.0000, 0.0000);
    add(0.352, 0.5765, 1.0000, 1.0000);
    add(0.361, 0.5765, 0.8745, 0.8745);
    add(0.370, 0.4510, 0.7608, 0.7843);
    add(0.380, 0.3255, 0.7098, 0.7098);
    add(0.389, 0.2275, 0.6196, 0.6196);
    add(0.398, 0.1373, 0.5608, 0.5608);
    add(0.407, 0.0353, 0.4863, 0.5451);
    add(0.417, 0.0000, 0.4118, 0.5216);
    add(0.426, 0.7529, 0.7529, 0.7529);
    add(0.435, 1.0000, 0.8510, 0.5608);
    add(0.444, 0.6510, 0.4588, 0.4510);
    add(0.454, 0.4000, 0.5020, 0.5020);
    add(0.463, 0.6196, 0.3843, 0.7098);
    add(0.472, 0.8275, 0.4745, 0.0000);
    add(0.481, 0.5765, 0.0000, 0.5765);
    add(0.491, 0.2588, 0.6196, 0.6863);
    add(0.500, 0.3373, 0.0863, 0.5608);
    add(0.509, 0.0000, 0.7843, 0.0000);
    add(0.519, 0.4353, 0.8275, 1.0000);
    add(0.528, 1.0000, 1.0000, 0.7765);
    add(0.537, 0.8510, 1.0000, 0.7765);
    add(0.546, 0.7765, 1.0000, 0.7765);
    add(0.556, 0.6353, 1.0000, 0.7765);
    add(0.565, 0.5608, 1.0000, 0.7765);
    add(0.574, 0.3765, 1.0000, 0.7765);
    add(0.583, 0.2706, 1.0000, 0.7765);
    add(0.593, 0.1843, 1.0000, 0.7765);
    add(0.602, 0.1216, 1.0000, 0.7765);
    add(0.611, 0.0000, 1.0000, 0.6118);
    add(0.620, 0.0000, 0.9020, 0.4588);
    add(0.630, 0.0000, 0.8275, 0.3216);
    add(0.639, 0.0000, 0.7451, 0.2196);
    add(0.648, 0.0000, 0.6706, 0.1373);
    add(0.657, 0.3020, 0.7608, 1.0000);
    add(0.667, 0.3020, 0.6510, 1.0000);
    add(0.676, 0.1255, 0.5765, 0.8353);
    add(0.685, 0.1451, 0.4863, 0.6706);
    add(0.694, 0.1451, 0.4000, 0.5843);
    add(0.704, 0.0863, 0.3255, 0.5255);
    add(0.713, 0.8157, 0.8157, 0.8745);
    add(0.722, 1.0000, 0.8196, 0.1333);
    add(0.731, 0.7216, 0.7216, 0.8157);
    add(0.741, 0.6510, 0.3255, 0.3020);
    add(0.750, 0.3373, 0.3451, 0.3765);
    add(0.759, 0.6196, 0.3098, 0.7098);
    add(0.769, 0.6706, 0.3608, 0.0000);
    add(0.778, 0.4588, 0.3098, 0.2706);
    add(0.787, 0.2588, 0.5098, 0.5843);
    add(0.796, 0.2588, 0.0000, 0.4000);
    add(0.806, 0.0000, 0.4863, 0.0000);
    add(0.815, 0.4353, 0.6706, 0.9765);
    add(0.824, 0.0000, 0.7255, 1.0000);
    add(0.833, 0.0000, 0.6275, 1.0000);
    add(0.843, 0.0000, 0.5608, 1.0000);
    add(0.852, 0.0000, 0.5020, 1.0000);
    add(0.861, 0.0000, 0.4196, 1.0000);
    add(0.870, 0.3255, 0.3608, 0.9451);
    add(0.880, 0.4706, 0.3608, 0.8863);
    add(0.889, 0.5373, 0.3098, 0.8863);
    add(0.898, 0.6275, 0.2118, 0.8275);
    add(0.907, 0.7020, 0.1216, 0.8275);
    add(0.917, 0.7020, 0.1216, 0.7255);
    add(0.926, 0.7020, 0.0510, 0.6510);
    add(0.935, 0.7373, 0.0510, 0.5255);
    add(0.944, 0.7765, 0.0000, 0.4000);
    add(0.954, 0.8000, 0.0000, 0.3451);
    add(0.963, 0.8196, 0.0000, 0.3098);
    add(0.972, 0.8510, 0.0000, 0.2706);
    add(0.981, 0.8745, 0.0000, 0.2196);
    add(0.991, 0.9020, 0.0000, 0.1765);
    add(1.000, 0.9216, 0.0000, 0.1451);
}

void RGBPalette::set_cpk_rasmol()
{
    add(0.000, 1.0000, 1.0000, 1.0000);
    add(0.009, 1.0000, 0.7529, 0.7922);
    add(0.019, 0.6941, 0.1294, 0.1294);
    add(0.028, 1.0000, 0.0745, 0.5725);
    add(0.037, 0.0000, 1.0000, 0.0000);
    add(0.046, 0.7804, 0.7804, 0.7804);
    add(0.056, 0.5608, 0.5608, 1.0000);
    add(0.065, 0.9373, 0.0000, 0.0000);
    add(0.074, 0.8549, 0.6431, 0.1216);
    add(0.083, 1.0000, 0.0745, 0.5725);
    add(0.093, 0.0000, 0.0000, 1.0000);
    add(0.102, 0.1294, 0.5412, 0.1294);
    add(0.111, 0.5020, 0.5020, 0.5647);
    add(0.120, 0.8549, 0.6431, 0.1216);
    add(0.130, 1.0000, 0.6431, 0.0000);
    add(0.139, 1.0000, 0.7804, 0.1922);
    add(0.148, 0.0000, 1.0000, 0.0000);
    add(0.157, 1.0000, 0.0745, 0.5725);
    add(0.167, 1.0000, 0.0745, 0.5725);
    add(0.176, 0.5020, 0.5020, 0.5647);
    add(0.185, 1.0000, 0.0745, 0.5725);
    add(0.194, 0.5020, 0.5020, 0.5647);
    add(0.204, 1.0000, 0.0745, 0.5725);
    add(0.213, 0.5020, 0.5020, 0.5647);
    add(0.222, 0.5020, 0.5020, 0.5647);
    add(0.231, 1.0000, 0.6431, 0.0000);
    add(0.241, 1.0000, 0.0745, 0.5725);
    add(0.250, 0.6431, 0.1647, 0.1647);
    add(0.259, 0.6431, 0.1647, 0.1647);
    add(0.269, 0.6431, 0.1647, 0.1647);
    add(0.278, 1.0000, 0.0745, 0.5725);
    add(0.287, 1.0000, 0.0745, 0.5725);
    add(0.296, 1.0000, 0.0745, 0.5725);
    add(0.306, 1.0000, 0.0745, 0.5725);
    add(0.315, 0.6431, 0.1647, 0.1647);
    add(0.324, 1.0000, 0.0745, 0.5725);
    add(0.333, 1.0000, 0.0745, 0.5725);
    add(0.343, 1.0000, 0.0745, 0.5725);
    add(0.352, 1.0000, 0.0745, 0.5725);
    add(0.361, 1.0000, 0.0745, 0.5725);
    add(0.370, 1.0000, 0.0745, 0.5725);
    add(0.380, 1.0000, 0.0745, 0.5725);
    add(0.389, 1.0000, 0.0745, 0.5725);
    add(0.398, 1.0000, 0.0745, 0.5725);
    add(0.407, 1.0000, 0.0745, 0.5725);
    add(0.417, 1.0000, 0.0745, 0.5725);
    add(0.426, 0.5020, 0.5020, 0.5647);
    add(0.435, 1.0000, 0.0745, 0.5725);
    add(0.444, 1.0000, 0.0745, 0.5725);
    add(0.454, 1.0000, 0.0745, 0.5725);
    add(0.463, 1.0000, 0.0745, 0.5725);
    add(0.472, 1.0000, 0.0745, 0.5725);
    add(0.481, 0.6235, 0.1216, 0.9373);
    add(0.491, 1.0000, 0.0745, 0.5725);
    add(0.500, 1.0000, 0.0745, 0.5725);
    add(0.509, 1.0000, 0.6431, 0.0000);
    add(0.519, 1.0000, 0.0745, 0.5725);
    add(0.528, 1.0000, 0.0745, 0.5725);
    add(0.537, 1.0000, 0.0745, 0.5725);
    add(0.546, 1.0000, 0.0745, 0.5725);
    add(0.556, 1.0000, 0.0745, 0.5725);
    add(0.565, 1.0000, 0.0745, 0.5725);
    add(0.574, 1.0000, 0.0745, 0.5725);
    add(0.583, 1.0000, 0.0745, 0.5725);
    add(0.593, 1.0000, 0.0745, 0.5725);
    add(0.602, 1.0000, 0.0745, 0.5725);
    add(0.611, 1.0000, 0.0745, 0.5725);
    add(0.620, 1.0000, 0.0745, 0.5725);
    add(0.630, 1.0000, 0.0745, 0.5725);
    add(0.639, 1.0000, 0.0745, 0.5725);
    add(0.648, 1.0000, 0.0745, 0.5725);
    add(0.657, 1.0000, 0.0745, 0.5725);
    add(0.667, 1.0000, 0.0745, 0.5725);
    add(0.676, 1.0000, 0.0745, 0.5725);
    add(0.685, 1.0000, 0.0745, 0.5725);
    add(0.694, 1.0000, 0.0745, 0.5725);
    add(0.704, 1.0000, 0.0745, 0.5725);
    add(0.713, 1.0000, 0.0745, 0.5725);
    add(0.722, 0.8549, 0.6431, 0.1216);
    add(0.731, 1.0000, 0.0745, 0.5725);
    add(0.741, 1.0000, 0.0745, 0.5725);
    add(0.750, 1.0000, 0.0745, 0.5725);
    add(0.759, 1.0000, 0.0745, 0.5725);
    add(0.769, 1.0000, 0.0745, 0.5725);
    add(0.778, 1.0000, 0.0745, 0.5725);
    add(0.787, 1.0000, 0.0745, 0.5725);
    add(0.796, 1.0000, 0.0745, 0.5725);
    add(0.806, 1.0000, 0.0745, 0.5725);
    add(0.815, 1.0000, 0.0745, 0.5725);
    add(0.824, 1.0000, 0.0745, 0.5725);
    add(0.833, 1.0000, 0.0745, 0.5725);
    add(0.843, 1.0000, 0.0745, 0.5725);
    add(0.852, 1.0000, 0.0745, 0.5725);
    add(0.861, 1.0000, 0.0745, 0.5725);
    add(0.870, 1.0000, 0.0745, 0.5725);
    add(0.880, 1.0000, 0.0745, 0.5725);
    add(0.889, 1.0000, 0.0745, 0.5725);
    add(0.898, 1.0000, 0.0745, 0.5725);
    add(0.907, 1.0000, 0.0745, 0.5725);
    add(0.917, 1.0000, 0.0745, 0.5725);
    add(0.926, 1.0000, 0.0745, 0.5725);
    add(0.935, 1.0000, 0.0745, 0.5725);
    add(0.944, 1.0000, 0.0745, 0.5725);
    add(0.954, 1.0000, 0.0745, 0.5725);
    add(0.963, 1.0000, 0.0745, 0.5725);
    add(0.972, 1.0000, 0.0745, 0.5725);
    add(0.981, 1.0000, 0.0745, 0.5725);
    add(0.991, 1.0000, 0.0745, 0.5725);
    add(1.000, 1.0000, 0.0745, 0.5725);
}

void RGBPalette::set_difference()
{
    add(0.000, 0.0000, 0.0000, 1.0000);
    add(0.500, 1.0000, 1.0000, 1.0000);
    add(1.000, 1.0000, 0.0000, 0.0000);
}

void RGBPalette::set_gray()
{
    add(0.000, 0.0000, 0.0000, 0.0000);
    add(1.000, 1.0000, 1.0000, 1.0000);
}

void RGBPalette::set_hot()
{
    add(0.000, 0.0000, 0.0000, 1.0000);
    add(0.250, 0.0000, 1.0000, 1.0000);
    add(0.500, 0.0000, 1.0000, 0.0000);
    add(0.750, 1.0000, 1.0000, 0.0000);
    add(1.000, 1.0000, 0.0000, 0.0000);
}

void RGBPalette::set_hot_and_cold()
{    
    add(0.000, 0.0000, 1.0000, 1.0000);
    add(0.450, 0.0000, 0.0000, 1.0000);
    add(0.500, 0.0000, 0.0000, 0.4980);
    add(0.550, 1.0000, 0.0000, 0.0000);
    add(1.000, 1.0000, 1.0000, 0.0000);
}

void RGBPalette::set_hot_desaturated()
{
    add(0.000, 0.2784, 0.2784, 0.8588);
    add(0.143, 0.0000, 0.0000, 0.3569);
    add(0.285, 0.0000, 1.0000, 1.0000);
    add(0.429, 0.0000, 0.4980, 0.0000);
    add(0.571, 1.0000, 1.0000, 0.0000);
    add(0.714, 1.0000, 0.3765, 0.0000);
    add(0.857, 0.4196, 0.0000, 0.0000);
    add(1.000, 0.8784, 0.2980, 0.2980);
}

void RGBPalette::set_levels()
{
    add(0.000, 1.0000, 0.0000, 0.0000);
    add(0.030, 0.0000, 1.0000, 0.0000);
    add(0.070, 0.0000, 0.0000, 1.0000);
    add(0.100, 0.0000, 1.0000, 1.0000);
    add(0.140, 1.0000, 0.0000, 1.0000);
    add(0.170, 1.0000, 1.0000, 0.0000);
    add(0.210, 1.0000, 0.5294, 0.0000);
    add(0.240, 1.0000, 0.0000, 0.5294);
    add(0.280, 0.6588, 0.6588, 0.6588);
    add(0.310, 1.0000, 0.2667, 0.2667);
    add(0.340, 0.3882, 1.0000, 0.3882);
    add(0.380, 0.3882, 0.3882, 1.0000);
    add(0.410, 0.1569, 0.6471, 0.6471);
    add(0.450, 1.0000, 0.3882, 1.0000);
    add(0.480, 1.0000, 1.0000, 0.3882);
    add(0.520, 1.0000, 0.6667, 0.3882);
    add(0.550, 0.6667, 0.3098, 1.0000);
    add(0.590, 0.5882, 0.0000, 0.0000);
    add(0.620, 0.0000, 0.5882, 0.0000);
    add(0.660, 0.0000, 0.0000, 0.5882);
    add(0.690, 0.0000, 0.4275, 0.4275);
    add(0.720, 0.5882, 0.0000, 0.5882);
    add(0.760, 0.5882, 0.5882, 0.0000);
    add(0.790, 0.5882, 0.3294, 0.0000);
    add(0.830, 0.6275, 0.0000, 0.3098);
    add(0.860, 1.0000, 0.4078, 0.1098);
    add(0.900, 0.0000, 0.6667, 0.3176);
    add(0.930, 0.2667, 1.0000, 0.4863);
    add(0.970, 0.0000, 0.5098, 1.0000);
    add(1.000, 0.5098, 0.0000, 1.0000);
}

void RGBPalette::set_orangehot()
{
    add(0.000, 0.0000, 0.0000, 0.0000);
    add(0.333, 0.4980, 0.0000, 0.0000);
    add(0.666, 1.0000, 0.4980, 0.0000);
    add(1.000, 1.0000, 1.0000, 1.0000);
}

void RGBPalette::set_rainbow()
{
    add(0.000, 1.0000, 0.0000, 1.0000);
    add(0.200, 0.0000, 0.0000, 1.0000);
    add(0.400, 0.0000, 1.0000, 1.0000);
    add(0.600, 0.0000, 1.0000, 0.0000);
    add(0.800, 1.0000, 1.0000, 0.0000);
    add(1.000, 1.0000, 0.0000, 0.0000);
}

void RGBPalette::set_xray()
{
    add(0.000, 1.0000, 1.0000, 1.0000);
    add(1.000, 0.0000, 0.0000, 0.0000);
}
