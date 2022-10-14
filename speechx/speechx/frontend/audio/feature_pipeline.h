// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// todo refactor later (SGoat)

#pragma once

#include "frontend/audio/assembler.h"
#include "frontend/audio/audio_cache.h"
#include "frontend/audio/data_cache.h"
#include "frontend/audio/fbank.h"
#include "frontend/audio/feature_cache.h"
#include "frontend/audio/frontend_itf.h"
#include "frontend/audio/linear_spectrogram.h"
#include "frontend/audio/normalizer.h"

namespace ppspeech {
struct FeaturePipelineOptions {
    std::string cmvn_file;
    bool to_float32;  // true, only for linear feature
    bool use_fbank;
    LinearSpectrogramOptions linear_spectrogram_opts;
    kaldi::FbankOptions fbank_opts;
    FeatureCacheOptions feature_cache_opts;
    AssemblerOptions assembler_opts;

    FeaturePipelineOptions()
        : cmvn_file(""),
          to_float32(false),  // true, only for linear feature
          use_fbank(true),
          linear_spectrogram_opts(),
          fbank_opts(),
          feature_cache_opts(),
          assembler_opts() {}
};

class FeaturePipeline : public FrontendInterface {
  public:
    explicit FeaturePipeline(const FeaturePipelineOptions& opts);
    virtual void Accept(const kaldi::VectorBase<kaldi::BaseFloat>& waves) {
        base_extractor_->Accept(waves);
    }
    virtual bool Read(kaldi::Vector<kaldi::BaseFloat>* feats) {
        return base_extractor_->Read(feats);
    }
    virtual size_t Dim() const { return base_extractor_->Dim(); }
    virtual void SetFinished() { base_extractor_->SetFinished(); }
    virtual bool IsFinished() const { return base_extractor_->IsFinished(); }
    virtual void Reset() { base_extractor_->Reset(); }

    const FeaturePipelineOptions& Config() { return opts_; }

    const BaseFloat FrameShift() const {
        return opts_.fbank_opts.frame_opts.frame_shift_ms;
    }
    const BaseFloat FrameLength() const {
        return opts_.fbank_opts.frame_opts.frame_length_ms;
    }
    const BaseFloat SampleRate() const {
        return opts_.fbank_opts.frame_opts.samp_freq;
    }

  private:
    FeaturePipelineOptions opts_;
    std::unique_ptr<FrontendInterface> base_extractor_;
};

}  // namespace ppspeech
