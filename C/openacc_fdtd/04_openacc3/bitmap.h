/**
 * @file bitmap.h
 * @brief Class for Bitmap format file
 *
 * Class for Bitmap format file
 *
 * @author Takashi Shimokawabe
 * @date 2010/12/12 Created
 * @version 0.1.0
 *
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <cstring>
#include <map>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


class BitmapBase;

/**
 * @brief Bitmap Header
 * 
 * Bitmap Header
 */
class BitmapHeader {
    friend class BitmapBase;
public:
    BitmapHeader();
    ~BitmapHeader();

private:
    void init();

public:
    /**
     * @brief set up format of image
     * @param[in] width  width of image
     * @param[in] height height of image
     * @param[in] bit_count bit const of image (8, 24 or 32 is supported)
     * @param[in] palette RGBQUAD palette data (this field is required for bit const == 8)
     * @retval 1 success
     * @retval 0 failure
     */    
    bool set(int width, int height, int bit_count, const unsigned char *palette);
    bool read(FILE *fp);
    bool write(FILE *fp) const;
    
    int width() const;
    int height() const;
    int bit_count() const;
    size_t image_size() const;
    size_t image_offset() const;
    size_t total_data_size() const;
    
    size_t linesize() const; // byte
    size_t palette_size() const; // byte
    bool has_palette() const;
    bool is_fullcolor() const;
    
    bool is_bitmap() const;    
    const unsigned char *palette() const;
    
    /**
     * @brief return supported bit count
     * @param[in] bit_count bit const of image (8, 24 or 32)
     * @retval 2 supported and not need palette (24 or 32 bit)
     * @retval 1 supported and required palette (8 bit)
     * @retval 0 not supported
     */
    int bit_count_is_supported(int bit_count) const;
    bool width_height_are_supported(int width, int heighte) const;
    
private:
    // Bitmap File Header (14 byte)
    uint16_t bfType_;           //!< "BM"
    uint32_t bfSize_;           //!< file size [byte]
    uint16_t bfReserved1_;      //!< 0
    uint16_t bfReserved2_;      //!< 0
    uint32_t bfOffBits_;        //!< offset to image data [byte]

    // Bitmap Information Header (40 byte)
    uint32_t biSize_;           //!< 40 [byte]
    int32_t  biWidth_;          //!< [pixel]
    int32_t  biHeight_;         //!< [pixel]
    uint16_t biPlanes_;         //!< 1
    uint16_t biBitCount_;       //!< support 8, 24 or 32 bit
    uint32_t biCompression_;    //!< support 0 (No Copmression) only
    uint32_t biSizeImage_;      //!< image data size [byte]
    int32_t  biXPixPerMeter_;   //!< 0 [ppm]
    int32_t  biYPixPerMeter_;   //!< 0 [ppm]
    uint32_t biClrUsed_;        //!< support 0
    uint32_t biCirImportant_;   //!< support 0

    // Bitmap Palette Data (RGBQUAD)
    // When biBitCount == 1, 4, or 8, Palette Data exist.
    unsigned char *palette_;
    
    bool is_bitmap_;
private:
    // Disallow the copy constructor and assignment operator
    BitmapHeader(const BitmapHeader&);
    void operator=(const BitmapHeader&);
};




/**
 * @brief Base of Bitmap format I/O
 * 
 * Base of Bitmap format I/O
 */
class BitmapBase {
public:
    /**
     * @brief constructor
     */
    BitmapBase();
    explicit BitmapBase(const char *file);
    virtual ~BitmapBase();

    virtual bool open(const char *file);
    const char *file_name() const;

    int width() const;
    int height() const;
    int bit_count() const;
    size_t image_data_size() const;   // rename
    size_t image_data_offset() const; // rename
    size_t size() const;              // rename

    size_t linesize() const; // byte
    size_t palette_size() const; // byte
    bool has_palette() const;
    bool is_fullcolor() const;
    bool is_bitmap() const;
    const unsigned char *palette() const;
    
public:    
    operator void *() const;
    bool operator !() const;
    bool is_good() const;
    bool is_failure() const;

protected:    
    void set_error(int error = 1);
    void clear_error();

protected:
    const char *file_;
    BitmapHeader *header_;
    
private:
    int error_;

    // Disallow the copy constructor and assignment operator
    BitmapBase(const BitmapBase&);
    void operator=(const BitmapBase&);
};



class BitmapPalette;
class RGBPalette;

/**
 * @brief Bitmap Writer
 * 
 * Bitmap Writer
 */
class BitmapWriter : public BitmapBase {
public:
    BitmapWriter();
    explicit BitmapWriter(const char *file);
    ~BitmapWriter();

public:
    // 0.0 <= r, g, b, <= 1.0
    template <typename T>
    int write_rgb(int width, int height, const T *r, const T *g, const T *b, int bit_count = 24);
    // pmin <= p <= pmax
    template <typename T>
    int write_rgb(int width, int height, const T *p, T pmin, T pmax,
                  const RGBPalette &palette, int bit_count = 24);
    // min(p) <= p <= max(p) (auto)
    template <typename T>    
    int write_rgb(int width, int height, const T *p,
                  const RGBPalette &palette, int bit_count = 24);
    // 0.0 <= p <= 1.0
    template <typename T>
    int write_8bit(int width, int height, const T *p, const unsigned char *palette);
    // 0.0 <= p <= 1.0
    template <typename T>
    int write_8bit(int width, int height, const T *p, const BitmapPalette &palette);
    
private:
    // Disallow the copy constructor and assignment operator
    BitmapWriter(const BitmapWriter&);
    void operator=(const BitmapWriter&);
};



/**
 * @brief Bitmap Reader
 * 
 * Bitmap Reader
 */
class BitmapReader : public BitmapBase {
public:
    /**
     * @brief constructor
     */
    BitmapReader();
    explicit BitmapReader(const char *file);
    ~BitmapReader();

public:
    bool open(const char *file);
    
private:
    bool read_header();
    
public:
    template <typename T>
    int read_rgb(int size, T *r, T *g, T *b);
    template <typename T>
    int read_8bit(int size, T *p, unsigned char *palette = NULL);
    
private:
    // Disallow the copy constructor and assignment operator
    BitmapReader(const BitmapReader&);
    void operator=(const BitmapReader&);
};




/**
 * @brief Bitmap Palette for 256 colors
 * 
 * Bitmap Palette for 256 colors
 */
class BitmapPalette {
public:
    enum PALETTE {
        RAINBOW,
        SEISMIC,
        HOT
    };
    
    BitmapPalette();
    explicit BitmapPalette(PALETTE palette);
    ~BitmapPalette();    
    
private:
    bool make(PALETTE palette);

public:
    const unsigned char *data() const;
    size_t size() const;

private:    
    unsigned char *data_;
    size_t size_;
    
private:
    // Disallow the copy constructor and assignment operator
    BitmapPalette(const BitmapPalette&);
    void operator=(const BitmapPalette&);
};



/**
 * @brief RGB Palette
 * 
 * RGB Palette
 */
class RGBPalette {
public:
    struct RGB {
        double r, g, b;
    };
    typedef std::map<double, struct RGB> RGBmap;

    enum PRESET {
        BLUEHOT,
        CALEBLACK,
        CALEWHITE,
        CONTOURED,
        CPK_JMOL,
        CPK_RASMOL,
        DIFFERENCE,
        GRAY,
        HOT,
        HOT_AND_COLD,
        HOT_DESATURATED,
        LEVELS,
        ORANGEHOT,
        RAINBOW,
        XRAY           
    };
    
public:    
    RGBPalette();
    explicit RGBPalette(PRESET preset);
    ~RGBPalette();    

public:
    int add(double value, double r, double g, double b);
    void set(PRESET preset);
    void clear();

    template <typename T>
    void rgb(double value, double vmin, double vmax, T *r, T *g, T *b) const;
    
    /**
     * @brief generate bitmap rgbquad palette
     * @param[in] nstep number of palette steps (step = 2 (1bit), 16 (4bit) or 256 (8bit))
     * @retval 1 success
     * @retval 0 failure
     */
    void generate_bitmap_rgbquad_palette(int nstep, unsigned char *palette) const;

private:
    void set_bluehot();
    void set_caleblack();
    void set_calewhite();
    void set_contoured();
    void set_cpk_jmol();
    void set_cpk_rasmol();
    void set_difference();
    void set_gray();
    void set_hot();
    void set_hot_and_cold();
    void set_hot_desaturated();
    void set_levels();
    void set_orangehot();
    void set_rainbow();
    void set_xray();
    
private:
    RGBmap rgbmap;
    
private:
    // Disallow the copy constructor and assignment operator
    RGBPalette(const RGBPalette&);
    void operator=(const RGBPalette&);
};



#endif /* BITMAP_H */


