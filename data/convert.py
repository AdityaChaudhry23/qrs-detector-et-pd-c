import wfdb, pandas as pd

record = wfdb.rdrecord('203')
pd.DataFrame(record.p_signal, columns=record.sig_name).to_csv('203.csv', index=False)

ann = wfdb.rdann('203', 'atr')
pd.DataFrame({'sample': ann.sample, 'symbol': ann.symbol}).to_csv('203.ann.csv', index=False)
