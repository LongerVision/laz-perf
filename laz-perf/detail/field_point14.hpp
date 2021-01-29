/*
===============================================================================

  FILE:  field_point14.hpp

  CONTENTS:


  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
    uday.karan@gmail.com - Hobu, Inc.

  COPYRIGHT:

    (c) 2007-2014, martin isenburg, rapidlasso - tools to catch reality
    (c) 2014, Uday Verma, Hobu, Inc.

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  CHANGE HISTORY:

===============================================================================
*/

#include <cmath>

#ifndef __las_hpp__
#error Cannot directly include this file, this is a part of las.hpp
#endif

namespace laszip {
namespace formats {

namespace {
    const U8 number_return_map_6ctx[16][16] = {
        {  0,  1,  2,  3,  4,  5,  3,  4,  4,  5,  5,  5,  5,  5,  5,  5 },
        {  1,  0,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3 },
        {  2,  1,  2,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3 },
        {  3,  3,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },
        {  4,  3,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },
        {  5,  3,  4,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },
        {  3,  3,  4,  4,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4 },
        {  4,  3,  4,  4,  4,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4,  4 },
        {  4,  3,  4,  4,  4,  4,  4,  4,  5,  4,  4,  4,  4,  4,  4,  4 },
        {  5,  3,  4,  4,  4,  4,  4,  4,  4,  5,  4,  4,  4,  4,  4,  4 },
        {  5,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  4,  4,  4,  4,  4 },
        {  5,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  4,  4,  4 },
        {  5,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  4,  4 },
        {  5,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  4 },
        {  5,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5 },
        {  5,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5 }
    };
    const U8 number_return_level_8ctx[16][16] =
    {
        {  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7 },
        {  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  7,  7,  7 },
        {  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  7,  7 },
        {  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  7 },
        {  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7 },
        {  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  7 },
        {  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7,  7 },
        {  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7,  7 },
        {  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6,  7 },
        {  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5,  6 },
        {  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4,  5 },
        {  7,  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4 },
        {  7,  7,  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2,  3 },
        {  7,  7,  7,  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1,  2 },
        {  7,  7,  7,  7,  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0,  1 },
        {  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  5,  4,  3,  2,  1,  0 }
    };
} // unnamed namespace

// Teach packers how to pack unpack the point14 struct
//
template<>
struct packers<las::point14>
{
    inline static las::point14 unpack(const char *in)
    {
        las::point14 p;

        p.setX(packers<int32_t>::unpack(in));              in += sizeof(int32_t);
        p.setY(packers<int32_t>::unpack(in));              in += sizeof(int32_t);
        p.setZ(packers<int32_t>::unpack(in));              in += sizeof(int32_t);
        p.setIntensity(packers<uint16_t>::unpack(in));     in += sizeof(uint16_t);
        p.setReturns(packers<uint8_t>::unpack(in));        in += sizeof(uint8_t);
        p.setFlags(packers<uint8_t>::unpack(in));          in += sizeof(uint8_t);
        p.setClassification(packers<uint8_t>::unpack(in)); in += sizeof(uint8_t);
        p.setUserData(packers<uint8_t>::unpack(in));       in += sizeof(uint8_t);
        p.setScanAngle(packers<int16_t>::unpack(in));      in += sizeof(int16_t);
        p.setPointSourceID(packers<uint16_t>::unpack(in)); in += sizeof(uint16_t);
        p.setGpsTime(packers<double>::unpack(in));
        return p;
    }

    inline static void pack(const las::point14& p, char *buffer)
    {
        packers<uint32_t>::pack(p.x(), buffer);                buffer += sizeof(int32_t);
        packers<uint32_t>::pack(p.y(), buffer);                buffer += sizeof(int32_t);
        packers<uint32_t>::pack(p.z(), buffer);                buffer += sizeof(int32_t);
        packers<uint16_t>::pack(p.intensity(), buffer);        buffer += sizeof(uint16_t);
        packers<uint8_t>::pack(p.returns(), buffer);          buffer += sizeof(uint8_t);
        packers<uint8_t>::pack(p.flags(), buffer);            buffer += sizeof(uint8_t);
        packers<uint8_t>::pack(p.classification(), buffer);   buffer += sizeof(uint8_t);
        packers<uint8_t>::pack(p.userData(), buffer);         buffer += sizeof(uint8_t);
        packers<int16_t>::pack(p.scanAngle(), buffer);        buffer += sizeof(int16_t);
        packers<uint16_t>::pack(p.pointSourceID(), buffer);    buffer += sizeof(uint16_t);
        packers<double>::pack(p.gpsTime(), buffer);
    }
}; // packers

// specialize field to compress point 10
//
template<>
struct field<las::point14>
{
    typedef las::point14 type;

    struct ChannelCtx
    {
        std::array<models::arithmetic *, 8> changed_values_model_;
        models::arithmetic *scanner_channel_model_;
        models::arithmetic *rn_gps_same_model_;
        std::array<models::arithmetic *, 16> nr_model_;
        std::array<models::arithmetic *, 16> rn_model_;

        std::array<models::arithmetic *, 64> class_model_;
        std::array<models::arithmetic *, 64> flag_model_;
        std::array<models::arithmetic *, 64> user_data_model_;

        models::arithmetic *gpstime_multi_model_;
        models::arithmetic *gpstime_0diff_model_;

        compressors::integer *dx_compr_;
        compressors::integer *dy_compr_;
        compressors::integer *z_compr_;
        compressors::integer *intensity_compr_;
        compressors::integer *scan_angle_compr_;
        compressors::integer *point_source_id_compr_;
        compressors::integer *gpstime_compr_;

        bool have_last_;
        las::point14 last_;
        std::array<uint16_t, 8> last_intensity_;
        std::array<int32_t, 8> last_z_;
        std::array<utils::streaming_median<int>, 12> last_x_diff_median5_;
        std::array<utils::streaming_median<int>, 12> last_y_diff_median5_;
        uint32_t last_gps_seq_;
        uint32_t next_gps_seq_;
        std::array<double, 4> last_gpstime_;
        std::array<int32_t, 4> last_gpstime_diff_;
        std::array<int32_t, 4> multi_extreme_counter_;
        bool gps_time_change_;
         
        ChannelCtx() : have_last_{false}, last_gps_seq_{0}, next_gps_seq_{0},
            last_gpstime_{}, last_gpstime_diff_{}, multi_extreme_counter_{},
            gps_time_change_{}
        {
            for (models::arithmetic * & m : changed_values_model_)
                m = new models::arithmetic(128);
            scanner_channel_model_ = new models::arithmetic(3);
            rn_gps_same_model_ = new models::arithmetic(13);
            for (models::arithmetic * & m : nr_model_)
                m = new models::arithmetic(16);
            for (models::arithmetic * & m : rn_model_)
                m = new models::arithmetic(16);

            for (models::arithmetic * & m : class_model_)
                m = new models::arithmetic(256);
            for (models::arithmetic * & m : flag_model_)
                m = new models::arithmetic(64);
            for (models::arithmetic * & m : user_data_model_)
                m = new models::arithmetic(256);

            gpstime_multi_model_ = new models::arithmetic(GpstimeMultiTotal);
            gpstime_0diff_model_ = new models::arithmetic(5);

            dx_compr_ = new compressors::integer(32, 2);
            dx_compr_->init();
            dy_compr_ = new compressors::integer(32, 22);
            dy_compr_->init();
            z_compr_ = new compressors::integer(32, 20);
            z_compr_->init();
            intensity_compr_ = new compressors::integer(16, 4);
            intensity_compr_->init();
            scan_angle_compr_ = new compressors::integer(16, 2);
            scan_angle_compr_->init();
            point_source_id_compr_ = new compressors::integer(16);
            point_source_id_compr_->init();
            gpstime_compr_ = new compressors::integer(32, 9);
            gpstime_compr_->init(); 

            for (auto& xd : last_x_diff_median5_)
                xd.init();
            for (auto& yd : last_y_diff_median5_)
                yd.init();
        }
    };  // ChannelCtx

    field() : last_channel_(-1)
    {}

    template<typename TStream>
    inline const char *compressWith(TStream& stream, const char *buf)
    {
        const las::point14 point = packers<las::point14>::unpack(buf);

        // We choose the model based on the scanner channel. In the laszip code, this
        // is called the context.
        int sc = point.scannerChannel();
        assert (sc >= 0 && sc <= 3);

        // don't have the first data for this channel yet, just push it to our have
        // last stuff and move on
        if (last_channel_ == -1)
        {
            ChannelCtx& c = chan_ctxs_[sc];
            stream.putBytes((const unsigned char*)buf, sizeof(las::point14));
            c.last_ = point;
            c.have_last_ = true;
            c.last_gpstime_[0] = point.gpsTime();
            last_channel_ = sc;

            for (auto& z : c.last_z_)
                z = point.z();
            for (auto& last_intensity : c.last_intensity_)
                last_intensity = point.intensity();

            return buf + sizeof(las::point14);
        }

        // Old is the context for the previous point.
        ChannelCtx& old = chan_ctxs_[last_channel_];
        // c is the context for this point.
        ChannelCtx& c = chan_ctxs_[sc];
        // If we haven't initialized the current context, do so.
        if (!c.have_last_)
        {
            c.have_last_ = true;
            ChannelCtx& old = chan_ctxs_[last_channel_];
            c.last_ = old.last_;
            c.last_z_ = old.last_z_;
            c.last_intensity_ = old.last_intensity_;
            c.last_gpstime_[0] = old.last_gpstime_[0];
            c.gps_time_change_ = old.gps_time_change_;
        }

        // There are 8 streams for the change bits based on the return number,
        // number of returns and a GPS time change. Calculate that stream number.
        // Called 'lpr' in the laszip code.
        int change_stream =
            (old.last_.returnNum() == 1) |                                // bit 0
            ((old.last_.returnNum() >= old.last_.numReturns()) << 1) |    // bit 1
            (old.gps_time_change_ << 2);                                  // bit 2

        bool returnNumIncrements = (point.returnNum() == (c.last_.returnNum() + 1) % 16);
        bool returnNumDecrements = (point.returnNum() == (c.last_.returnNum() + 15) % 16);
        bool returnNumChanges =
            (point.returnNum() != c.last_.returnNum()) &&
            !returnNumIncrements && !returnNumDecrements;

        int changed_values =
            ((returnNumIncrements || returnNumChanges) << 0) |
            ((returnNumDecrements || returnNumChanges) << 1) |
            ((point.numReturns() != c.last_.numReturns()) << 2) |
            ((point.scanAngle() != c.last_.scanAngle()) << 3) |
            ((point.uGpsTime() != c.last_.uGpsTime()) << 4) |
            ((point.pointSourceID() != c.last_.pointSourceID()) << 5) |
            ((sc != old.last_.scannerChannel()) << 6);

        auto& m = *c.changed_values_model_[change_stream];
        xy_enc_.encodeSymbol(*c.changed_values_model_[change_stream], changed_values);

        if (sc > old.last_.scannerChannel())
            xy_enc_.encodeSymbol(*c.scanner_channel_model_, sc - last_channel_ - 1);
        else if (sc < old.last_.scannerChannel())
            xy_enc_.encodeSymbol(*c.scanner_channel_model_, sc - last_channel_ - 1 + 4);
            
        if (point.numReturns() != c.last_.numReturns())
            xy_enc_.encodeSymbol(*c.nr_model_[c.last_.numReturns()], point.numReturns());

        if (returnNumChanges)
        {
            if (point.iGpsTime() != c.last_.iGpsTime())
                xy_enc_.encodeSymbol(*c.rn_model_[c.last_.returnNum()], point.returnNum());
            else
            {
                //ABELL - I believe this is broken for the case when diff == -15, as
                //  it results in a symbol of -1, which is coerced into an unsigned value.
                int diff = point.returnNum() - c.last_.returnNum();
                if (diff > 1)
                    xy_enc_.encodeSymbol(*c.rn_gps_same_model_, diff - 2);
                else
                    xy_enc_.encodeSymbol(*c.rn_gps_same_model_, diff - 2 + 16);
            }
        }

        // X and Y
        {
            uint32_t ctx =
                (point.iGpsTime() != c.last_.iGpsTime()) |
                (number_return_map_6ctx[point.numReturns()][point.returnNum()] << 1);
            int32_t median = c.last_x_diff_median5_[ctx].get();
            int32_t diff = point.x() - c.last_.x();
            c.dx_compr_->compress(xy_enc_, median, diff, point.numReturns() == 1);
            c.last_x_diff_median5_[ctx].add(diff);

            // Max of 20, low bit cleared to allow for numReturns change.
            uint32_t kbits = (std::min)(c.dx_compr_->getK(), 20U) & ~1;
            median = c.last_y_diff_median5_[ctx].get();
            diff = point.y() - c.last_.y();
            c.dy_compr_->compress(xy_enc_, median, diff, (point.numReturns() == 1) | kbits);
            c.last_y_diff_median5_[ctx].add(diff);
        }

        // Z
        {
            uint32_t kbits = (c.dx_compr_->getK() + c.dy_compr_->getK()) / 2;
            kbits = (std::min)(kbits, 18U) & ~1;
            uint32_t ctx = number_return_level_8ctx[point.numReturns()][point.returnNum()];
            c.z_compr_->compress(z_enc_, c.last_z_[ctx], point.z(),
                (point.numReturns() == 1) | kbits); 
            c.last_z_[ctx] = point.z();
        }

        // Classification
        {
            int32_t ctx =
                // This bit is supposed to represent an only return.
                ((point.returnNum() == 1) && (point.returnNum() >= point.numReturns())) |
                // Class 0 - 31, shifted.
                ((c.last_.classification() & 0x1F) << 1);

            class_enc_.encodeSymbol(*c.class_model_[ctx], point.classification());
        }

        // Flags
        {
            // This nonsense is to pack the flags, since we've already written the scanner
            // channel that's normally part of the flag byte.
            uint32_t flags =
                point.classFlags() |
                (point.scanDirFlag() << 4) |
                (point.eofFlag() << 5);
            uint32_t last_flags =
                c.last_.classFlags() |
                (c.last_.scanDirFlag() << 4) |
                (c.last_.eofFlag() << 5);

            flags_enc_.encodeSymbol(*c.flag_model_[last_flags], flags);
        }

        // Intensity
        {
            int32_t ctx =
                (point.iGpsTime() != c.last_.iGpsTime()) |
                ((point.returnNum() >= point.numReturns()) << 1) |
                ((point.returnNum() == 1) << 2);

            c.intensity_compr_->compress(intensity_enc_,
                c.last_intensity_[ctx], point.intensity(), ctx >> 1);
            c.last_intensity_[ctx] = point.intensity();
        }

        // Scan angle
        {
            if (point.scanAngle() != c.last_.scanAngle())
                c.scan_angle_compr_->compress(scan_angle_enc_, c.last_.scanAngle(),
                    point.scanAngle(), point.iGpsTime() != c.last_.iGpsTime());
        }

        // User data
        {
            int32_t ctx = c.last_.userData() / 4;
            user_data_enc_.encodeSymbol(*c.user_data_model_[ctx], point.userData());
        }

        // Point Source ID
        {
            if (point.pointSourceID() != c.last_.pointSourceID())
                c.point_source_id_compr_->compress(point_source_id_enc_,
                    c.last_.pointSourceID(), point.pointSourceID(), 0);
        }

        // GPS Time
        encodeGpsTime(point, c);

        last_channel_ = sc;
        c.gps_time_change_ = (point.gpsTime() != c.last_.gpsTime());
        c.last_ = point;
        return buf + sizeof(las::point14);
    }

    void encodeGpsTime(const las::point14& point, ChannelCtx& c)
    {
        auto findSeq = [&c](double gpstime, int start, int32_t& diff)
        {
            auto i64 = [](double d) { return *reinterpret_cast<int64_t *>(&d); };

            for (int i = start; i < 4; ++i)
            {
                int testseq = (c.last_gps_seq_ + i) & 0x3;
                int64_t diff64 = i64(gpstime) - i64(c.last_gpstime_[testseq]);
                diff = (int32_t)diff64;
                if (diff64 == diff)
                    return i;
            }
            return -1;
        };

        if (point.iGpsTime() == c.last_.iGpsTime())
            return;

        auto u64 = [](double d){ return *reinterpret_cast<uint64_t *>(&d); };

        loop: 
        if (c.last_gpstime_diff_[c.last_gps_seq_] == 0)
        {
            int32_t diff;  // Difference between current time and last time in the sequence,
                           // as 32 bit int.
            int idx = findSeq(point.gpsTime(), 0, diff);

            if (idx == 0)
            {
//std::cerr << "Encode 0!\n";
                gpstime_enc_.encodeSymbol(*c.gpstime_0diff_model_, 0);
                c.gpstime_compr_->compress(gpstime_enc_, 0, diff, 0);
                c.last_gpstime_diff_[c.last_gps_seq_] = diff;
                c.multi_extreme_counter_[c.last_gps_seq_] = 0;
            }
            else if (idx > 0)
            {
//std::cerr << "Encode Seq = " << (idx + 1) << "!\n";
                gpstime_enc_.encodeSymbol(*c.gpstime_0diff_model_, idx + 1);
                c.last_gps_seq_ = (c.last_gps_seq_ + idx) & 3;
                //ABELL - Seems like this is just going to get into the case above.
                //  and be done.
                goto loop;
            }
            else
            {
//std::cerr << "Encode New = 1!\n";
                gpstime_enc_.encodeSymbol(*c.gpstime_0diff_model_, 1);
                c.gpstime_compr_->compress(gpstime_enc_,
                    u64(c.last_gpstime_[c.last_gps_seq_]) >> 32, point.uGpsTime() >> 32, 8);
                gpstime_enc_.writeInt((uint32_t)(point.uGpsTime()));
                c.last_gps_seq_ = c.next_gps_seq_ = (c.next_gps_seq_ + 1) % 3;
                c.last_gpstime_diff_[c.last_gps_seq_] = 0;
                c.multi_extreme_counter_[c.last_gps_seq_] = 0;
            }
            c.last_gpstime_[c.last_gps_seq_] = point.gpsTime();
        }
        else  // Last diff nonzero.
        {
            auto i64 = [](double d) { return *reinterpret_cast<int64_t *>(&d); };

            int64_t diff64 = point.iGpsTime() - i64(c.last_gpstime_[c.last_gps_seq_]);
            int32_t diff = (int32_t)diff64;

            // 32 bit difference.
            if (diff64 == diff)
            {
                // Compute multiplier between current and last int difference.
//std::cerr << "Curr/last = " << (float)diff << "/" << (float)(c.last_gpstime_diff_[c.last_gps_seq_]) << "!\n";
                int32_t multi =
                    (int32_t)std::round(diff / (float)c.last_gpstime_diff_[c.last_gps_seq_]);
                if (multi > 0 && multi < GpstimeMulti)
                {
                    int tag;
                    if (multi == 1)
                        tag = 1;  // The case for regular spaced pulses.
                    else if (multi > 1 && multi < 10)
                        tag = 2;
                    else
                        tag = 3;
//std::cerr << "Encode multi = " << multi << "!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_, multi);
                    c.gpstime_compr_->compress(gpstime_enc_,
                        multi * c.last_gpstime_diff_[c.last_gps_seq_], diff, tag);
                    if (tag == 1)
                        c.multi_extreme_counter_[c.last_gps_seq_] = 0;
                }
                else if (multi >= GpstimeMulti)
                {
//std::cerr << "Encode multi MULTI!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_, GpstimeMulti);
                    c.gpstime_compr_->compress(gpstime_enc_,
                        GpstimeMulti * c.last_gpstime_diff_[c.last_gps_seq_], diff, 4);
                    c.multi_extreme_counter_[c.last_gps_seq_]++;
                    if (c.multi_extreme_counter_[c.last_gps_seq_] > 3)
                    {
                        c.multi_extreme_counter_[c.last_gps_seq_] = 0;
                        c.last_gpstime_diff_[c.last_gps_seq_] = diff;
                    }
                }
                else if (multi < 0 && multi > GpstimeMultiMinus)
                {
//std::cerr << "Encode multi minus = " << (GpstimeMulti - multi) << "!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_, GpstimeMulti - multi);
                    c.gpstime_compr_->compress(gpstime_enc_,
                        multi * c.last_gpstime_diff_[c.last_gps_seq_], diff, 5);
                }
                else if (multi <= GpstimeMultiMinus)
                {
//std::cerr << "Encode multi minus std = " << (GpstimeMulti - GpstimeMultiMinus) << "!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_,
                        GpstimeMulti - GpstimeMultiMinus);
                    c.gpstime_compr_->compress(gpstime_enc_,
                        GpstimeMultiMinus * c.last_gpstime_diff_[c.last_gps_seq_], diff, 6);
                    c.multi_extreme_counter_[c.last_gps_seq_]++;
                    if (c.multi_extreme_counter_[c.last_gps_seq_] > 3)
                    {
                        c.multi_extreme_counter_[c.last_gps_seq_] = 0;
                        c.last_gpstime_diff_[c.last_gps_seq_] = diff;
                    }
                }
                else if (multi == 0)
                {
//std::cerr << "Encode multi 0!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_, 0);
                    c.gpstime_compr_->compress(gpstime_enc_, 0, diff, 7);
                    c.multi_extreme_counter_[c.last_gps_seq_]++;
                    if (c.multi_extreme_counter_[c.last_gps_seq_] > 3)
                    {
                        c.multi_extreme_counter_[c.last_gps_seq_] = 0;
                        c.last_gpstime_diff_[c.last_gps_seq_] = diff;
                    }
                }
            }
            // Large difference
            else
            {
                int32_t diff;
                int idx = findSeq(point.gpsTime(), 1, diff);
                if (idx > 0)
                {
//std::cerr << "Encode FULL + = " << (GpstimeMultiCodeFull + idx) << "!\n";
                    gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_,
                        GpstimeMultiCodeFull + idx);
                    c.last_gps_seq_ = (c.last_gps_seq_ + idx) & 3;
                    goto loop;
                }
//std::cerr << "Encode FULL base = " << (GpstimeMultiCodeFull) << "!\n";
                gpstime_enc_.encodeSymbol(*c.gpstime_multi_model_, GpstimeMultiCodeFull);
                c.gpstime_compr_->compress(gpstime_enc_,
                    int32_t(u64(c.last_gpstime_[c.last_gps_seq_]) >> 32),
                    int32_t(point.uGpsTime() >> 32), 8);
//std::cerr << "Write raw = " << point.uGpsTime() << "!\n";
                gpstime_enc_.writeInt((uint32_t)(point.uGpsTime()));
                c.next_gps_seq_ = c.last_gps_seq_ = (c.next_gps_seq_ + 1) & 3;
                c.last_gpstime_diff_[c.last_gps_seq_] = 0;
                c.multi_extreme_counter_[c.last_gps_seq_] = 0;
            }
            c.last_gpstime_[c.last_gps_seq_] = point.gpsTime();
        }
    }

    template <typename TStream>
    void done(TStream& stream)
    {
        xy_enc_.done();
        z_enc_.done();
        class_enc_.done();
        flags_enc_.done();
        intensity_enc_.done();
        scan_angle_enc_.done();
        user_data_enc_.done();
        point_source_id_enc_.done();
        gpstime_enc_.done();

std::cerr << "XY bytes = " << xy_enc_.num_encoded() << "\n";
std::cerr << "Z bytes = " << z_enc_.num_encoded() << "\n";
std::cerr << "Class bytes = " << class_enc_.num_encoded() << "\n";
std::cerr << "Flags bytes = " << flags_enc_.num_encoded() << "\n";
std::cerr << "Intensity bytes = " << intensity_enc_.num_encoded() << "\n";
std::cerr << "Scan angle bytes = " << scan_angle_enc_.num_encoded() << "\n";
std::cerr << "User bytes = " << user_data_enc_.num_encoded() << "\n";
std::cerr << "Point source bytes = " << point_source_id_enc_.num_encoded() << "\n";
std::cerr << "GPStime bytes = " << gpstime_enc_.num_encoded() << "\n";

        stream << xy_enc_.num_encoded();
        stream << z_enc_.num_encoded();
        stream << class_enc_.num_encoded();
        stream << flags_enc_.num_encoded();
        stream << intensity_enc_.num_encoded();
        stream << scan_angle_enc_.num_encoded();
        stream << user_data_enc_.num_encoded();
        stream << point_source_id_enc_.num_encoded();
        stream << gpstime_enc_.num_encoded();

auto sum = [](const uint8_t *buf, uint32_t size)
{
    int32_t sum = 0;
    while (size--)
        sum += *buf++;
    return sum;
};

std::cerr << sum(xy_enc_.encoded_bytes(), xy_enc_.num_encoded()) << "!\n";
std::cerr << sum(z_enc_.encoded_bytes(), z_enc_.num_encoded()) << "!\n";
std::cerr << sum(class_enc_.encoded_bytes(), class_enc_.num_encoded()) << "!\n";
std::cerr << sum(flags_enc_.encoded_bytes(), flags_enc_.num_encoded()) << "!\n";
std::cerr << sum(intensity_enc_.encoded_bytes(), intensity_enc_.num_encoded()) << "!\n";
std::cerr << sum(scan_angle_enc_.encoded_bytes(), scan_angle_enc_.num_encoded()) << "!\n";
std::cerr << sum(user_data_enc_.encoded_bytes(), user_data_enc_.num_encoded()) << "!\n";
std::cerr << sum(point_source_id_enc_.encoded_bytes(), point_source_id_enc_.num_encoded()) << "!\n";
std::cerr << sum(gpstime_enc_.encoded_bytes(), gpstime_enc_.num_encoded()) << "!\n";
        stream.putBytes(xy_enc_.encoded_bytes(), xy_enc_.num_encoded());
        stream.putBytes(z_enc_.encoded_bytes(), z_enc_.num_encoded());
        if (class_enc_.num_encoded())
            stream.putBytes(class_enc_.encoded_bytes(), class_enc_.num_encoded());
        if (flags_enc_.num_encoded())
            stream.putBytes(flags_enc_.encoded_bytes(), flags_enc_.num_encoded());
        if (intensity_enc_.num_encoded())
            stream.putBytes(intensity_enc_.encoded_bytes(), intensity_enc_.num_encoded());
        if (scan_angle_enc_.num_encoded())
            stream.putBytes(scan_angle_enc_.encoded_bytes(), scan_angle_enc_.num_encoded());
        if (user_data_enc_.num_encoded())
            stream.putBytes(user_data_enc_.encoded_bytes(), user_data_enc_.num_encoded());
        if (point_source_id_enc_.num_encoded())
            stream.putBytes(point_source_id_enc_.encoded_bytes(),
                point_source_id_enc_.num_encoded());
        if (gpstime_enc_.num_encoded())
            stream.putBytes(gpstime_enc_.encoded_bytes(), gpstime_enc_.num_encoded());
    }

    static const int GpstimeMulti = 500;
    static const int GpstimeMultiMinus = -10;
    static const int GpstimeMultiCodeFull = GpstimeMulti - GpstimeMultiMinus + 1;
    static const int GpstimeMultiTotal = GpstimeMulti - GpstimeMultiMinus + 5;

    std::array<ChannelCtx, 4> chan_ctxs_;
    encoders::arithmetic<MemoryStream> xy_enc_;
    encoders::arithmetic<MemoryStream> z_enc_;
    encoders::arithmetic<MemoryStream> class_enc_;
    encoders::arithmetic<MemoryStream> flags_enc_;
    encoders::arithmetic<MemoryStream> intensity_enc_;
    encoders::arithmetic<MemoryStream> scan_angle_enc_;
    encoders::arithmetic<MemoryStream> user_data_enc_;
    encoders::arithmetic<MemoryStream> point_source_id_enc_;
    encoders::arithmetic<MemoryStream> gpstime_enc_;
    int last_channel_;
};

} // namespace formats
} // namespace laszip
