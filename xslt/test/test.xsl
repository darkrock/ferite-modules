<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
version="1.0">
<xsl:template match="/">
  <html>
  <head><title>Know Your Dromedaries</title></head>
  <body>
    <table bgcolor="#eeeeee" border="1">
    <tr>
    <th>Species</th>
    <th>No of Humps</th>
    <th>Disposition</th>
    </tr>
    <xsl:for-each select="dromedaries">
      <xsl:apply-templates select="./species" />
  </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

<xsl:template match="species">
  <tr>
  <td><xsl:value-of select="@name" /></td>
  <td><xsl:value-of select="./humps" /></td>
  <td><xsl:value-of select="./disposition" /></td>
  </tr>
</xsl:template>

</xsl:stylesheet>
