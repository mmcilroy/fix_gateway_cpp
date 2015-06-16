#pragma once

namespace fix {

const int begin_string = 8;
const int body_length = 9;
const int check_sum = 10;
const int msg_seq_num = 34;
const int msg_type = 35;
const int sender_comp_id = 49;
const int sender_sub_id = 50;
const int sending_time = 52;
const int target_comp_id = 56;
const int delim = (char)1;

const char delim_char = '|'; //(char)1;
const char delim_str[] = { delim_char, (char)0 };

}
