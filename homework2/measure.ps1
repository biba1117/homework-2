param([string]$Executable)
# Measures complete console runs, including startup and input/output.
' terms,mean_ms,min_ms,max_ms'.Trim()
foreach ($n in @(10,50,100,200)) {
    $parts = @([string]$n)
    for ($i=0; $i -lt $n; $i++) { $parts += '1'; $parts += [string]$i }
    $poly = $parts -join ' '
    $inputText = "$poly`n$poly`n1"
    $times = @()
    for ($trial=0; $trial -lt 5; $trial++) {
        $elapsed = Measure-Command { $inputText | & $Executable | Out-Null }
        if ($LASTEXITCODE -ne 0) { throw 'Program failed' }
        $times += $elapsed.TotalMilliseconds
    }
    $stats = $times | Measure-Object -Average -Minimum -Maximum
    '{0},{1:F3},{2:F3},{3:F3}' -f $n,$stats.Average,$stats.Minimum,$stats.Maximum
}
