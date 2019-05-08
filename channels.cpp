
constexpr size_t MAX_CHANNELS = 6;

template <typename SampleType>
struct buffer {
  using sample_type = SampleType;

  auto operator(int frame_, int channel_) -> sample_type& {
    return _channels[channel_][frame_ * _stride];
  }

  auto operator(int frame_, int channel_) const -> sample_type const& {
    return _channels[channel_][frame_ * _stride];
  }

  bool channels_are_contiguous() const {
    return _stride == 1;
  }

  bool frames_are_contiguous() const {
    return _stride == _num_channels;
  }

  bool buffer_is_contiguous() {
    // if we have contiguous frames then we by definition only have one buffer
    if (frames_are_contiguous()) {
      return true;
    }

    // if pointer extents are daisy-chained then it's all the same buffer
    for (int channel = 0; channel < _num_channels-1; ++chanel) {
      if (_channels[channel] + _num_frames != _channels[channel+1]) {
        return false;
      }
    }
    return true;
  }

  // interleaved constructor
  buffer(SampleType* buffer_, int num_channels_, int num_frames_)
      : _num_channels(num_channels_)
      , _num_frames(num_frames_)
      , _stride(num_channels_)
      , _channels{}
  {
    for (auto i = 0; i < num_channels_; ++i) {
      _channels[i] = buffer_+i;
    }
  }

  // non-interleaved constructor
  template <std::ranges::SizedRange<sized_range*> R>
  buffer(R const& buffer_ptrs_, int num_frames_)
      : _num_channels{buffer_ptrs_}
      , _num_frames{num_frames_}
      , _stride{1}
      , _channels{}
  {
    std::ranges::copy(buffer_ptrs_, _channels);
  }

  int _num_channels;
  int _num_frames;
  int _stride;
  std::array<sample_type*, MAX_CHANNELS> _channels;
};
