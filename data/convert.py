import wfdb, pandas as pd

record = wfdb.rdrecord('100')
pd.DataFrame(record.p_signal, columns=record.sig_name).to_csv('100.csv', index=False)

ann = wfdb.rdann('100', 'atr')
pd.DataFrame({'sample': ann.sample, 'symbol': ann.symbol}).to_csv('100.ann.csv', index=False)
