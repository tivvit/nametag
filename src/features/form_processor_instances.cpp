// This file is part of NameTag.
//
// Copyright 2013 by Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// NameTag is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// NameTag is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with NameTag.  If not, see <http://www.gnu.org/licenses/>.

#include "form_processor.h"
#include "utils/file_ptr.h"
#include "utils/input.h"
#include "utils/utf8.h"

namespace ufal {
namespace nametag {

// Helper functions defined as macros so that they can access arguments without passing them
#define apply_in_window(I, Feature) {                                        \
  ner_feature _feature = (Feature);                                          \
  if (_feature != ner_feature_unknown)                                       \
    for (int _i = 1; _i <= window && int(I) + _i < int(sentence.size); _i++) \
      sentence.features[int(I) + _i].emplace_back(_feature + _i - 1);        \
}

#define apply_outer_words_in_window(Feature) { \
  ner_feature _outer_feature = (Feature);      \
  if (_outer_feature != ner_feature_unknown)   \
    for (int _i = 1; _i <= window; _i++)       \
        apply_in_window(-_i, _outer_feature);  \
}

#define lookup_empty() /* lookup(string()) always returns */(window)


//////////////////////////////////////////////////////////
// Form processor instances (ordered lexicographically) //
//////////////////////////////////////////////////////////
namespace form_processors {

// PreviousEntity
class previous_entity : public form_processor {
 public:
  virtual void process_form(int form, ner_sentence& sentence, ner_feature* total_features, string& /*buffer*/) const override {
    if (form == 0) {
      apply_outer_words_in_window(lookup_empty());
    } else {
      auto& probs = sentence.probabilities[form-1].global;
      apply_in_window(form, lookup(sentence.words[form-1].form + to_string(probs.bilou[probs.best].entity * bilou_type_total + probs.best), total_features));
    }
  }
};

} // namespace form_processors

// Form processor factory method
form_processor* form_processor::create(const string& name) {
  if (name.compare("PreviousEntity") == 0) return new form_processors::previous_entity();
  return nullptr;
}

} // namespace nametag
} // namespace ufal