import pickle as pk
import pandas as pd


# 1. read the original data & split into training and target data
diabetic_data = pd.read_csv('data/diabetic_data.csv').drop('encounter_id', 1)
diabetic_data = diabetic_data.drop('patient_nbr', 1)
meta = {'total_instances': diabetic_data.shape[0], 'used_cols': {}}

# 2. device Numerical and Categorical columns
num_data = diabetic_data._get_numeric_data()
for col_name in num_data:
    if 'id' in col_name:
        num_data = num_data.drop(col_name, 1)
cate_data = diabetic_data.drop(num_data.columns, 1)

# 3. get information from Categorical columns
total_idx = 0
for col_name in cate_data.columns:
    meta['used_cols'][col_name] = dict(diabetic_data[col_name].describe())
    meta['used_cols'][col_name]['categories'] = []
    meta['used_cols'][col_name]['cate_cnt'] = {}
    meta['used_cols'][col_name]['cate_idx'] = {}
    idx = 0
    for unique_val in diabetic_data[col_name].unique():
        if 'diag_' in col_name or unique_val == 'None':
            continue
        meta['used_cols'][col_name]['categories'].append(str(unique_val))
        meta['used_cols'][col_name]['cate_cnt'][str(unique_val)] = diabetic_data[col_name].value_counts()[unique_val]
        if '?' in str(unique_val): continue
        meta['used_cols'][col_name]['cate_idx'][str(unique_val)] = idx
        idx = idx + 1
    total_idx = total_idx + idx

# 4. get information from Numerical columns
for col_name in num_data.columns:
    meta['used_cols'][col_name] = dict(diabetic_data[col_name].describe())
    total_idx = total_idx + 1

# 5. set information whether the column value is numeric or categorical
for col_name in diabetic_data.columns:
    cnt_table = diabetic_data[col_name].value_counts()
    if '?' in cnt_table.index:
        meta['used_cols'][col_name]['missing_cnt'] = cnt_table['?']
        total_idx = total_idx + 1
    if col_name in num_data.columns:
        meta['used_cols'][col_name]['data_type'] = 'numeric'
    else:
        meta['used_cols'][col_name]['data_type'] = 'categorical'
    if col_name == 'readmitted':
        meta['used_cols'][col_name]['data_type'] = 'target'


print(meta)

# 6. finally, write meta to a file
with open('dicts/meta.p', 'wb') as f:
    pk.dump(meta, f, pk.HIGHEST_PROTOCOL)


def get_ICD(icd_code_original):
    icd_code = icd_code_original.split('.')[0]
    icd_code_detail = '-1'
    is_diabete = 0
    is_uncontrolled = 0
    type = 2
    DIABETE_CODE = '250'
    icd_code_index = [('1', '139'), ('140', '239'), ('240', '279'), ('280', '289'), ('290', '319'),
                      ('320', '359'), ('360', '389'), ('390', '459'), ('460', '519'), ('520', '579'),
                      ('580', '629'), ('630', '679'), ('680', '709'), ('710', '739'), ('740', '759'),
                      ('760', '779'), ('780', '799'), ('800', '999'), ('E', 'V')]

    if icd_code_index[0][0] <= icd_code <= icd_code_index[0][1]:
        index = 0
    elif icd_code_index[1][0] <= icd_code <= icd_code_index[1][1]:
        index = 1
    elif icd_code_index[2][0] <= icd_code <= icd_code_index[2][1]:
        index = 2
        if icd_code == DIABETE_CODE:
            is_diabete = 1
            if '.' in icd_code_original:
                icd_code_detail = icd_code_original.split('.')[1]
                if len(icd_code_detail) == 2:
                    if icd_code_detail[1] == '0':
                        type = 1
                    elif icd_code_detail[1] == '1':
                        type = 0
                    elif icd_code_detail[1] == '2':
                        type = 1
                        is_uncontrolled = 1
                    elif icd_code_detail[1] == '3':
                        type = 0
                        is_uncontrolled = 1
                icd_code_detail = icd_code_detail[0]

    elif icd_code_index[3][0] <= icd_code <= icd_code_index[3][1]:
        index = 3
    elif icd_code_index[4][0] <= icd_code <= icd_code_index[4][1]:
        index = 4
    elif icd_code_index[5][0] <= icd_code <= icd_code_index[5][1]:
        index = 5
    elif icd_code_index[6][0] <= icd_code <= icd_code_index[6][1]:
        index = 6
    elif icd_code_index[7][0] <= icd_code <= icd_code_index[7][1]:
        index = 7
    elif icd_code_index[8][0] <= icd_code <= icd_code_index[8][1]:
        index = 8
    elif icd_code_index[9][0] <= icd_code <= icd_code_index[9][1]:
        index = 9
    elif icd_code_index[10][0] <= icd_code <= icd_code_index[10][1]:
        index = 10
    elif icd_code_index[11][0] <= icd_code <= icd_code_index[11][1]:
        index = 11
    elif icd_code_index[12][0] <= icd_code <= icd_code_index[12][1]:
        index = 12
    elif icd_code_index[13][0] <= icd_code <= icd_code_index[13][1]:
        index = 13
    elif icd_code_index[14][0] <= icd_code <= icd_code_index[14][1]:
        index = 14
    elif icd_code_index[15][0] <= icd_code <= icd_code_index[15][1]:
        index = 15
    elif icd_code_index[16][0] <= icd_code <= icd_code_index[16][1]:
        index = 16
    elif icd_code_index[17][0] <= icd_code <= icd_code_index[17][1]:
        index = 17
    elif icd_code_index[18][0] in icd_code:
        index = 18
    elif icd_code_index[18][1] in icd_code:
        index = 18
    else:
        index = 19

    return index, is_diabete, type, is_uncontrolled, int(icd_code_detail)

def print_icd_code_index():
    count=0
    for value in diabetic_data.diag_1:
        if value.split('.')[0] == '250':
            print(value, ": ", get_ICD(value))
            count = count + 1
    print(count)

if __name__ == "__main__":
    #print_icd_code_index()
    get_ICD
